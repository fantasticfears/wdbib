#include "spec_stateful_parser.h"

#include <string>

#include <absl/strings/ascii.h>
#include <absl/strings/match.h>
#include <absl/strings/numbers.h>
#include <absl/strings/str_cat.h>
#include <absl/strings/str_split.h>
#include <fmt/format.h>

#include "errors.h"
#include "wdbib_data.h"
#include "storage/data_file.h"

namespace wdbib {

using namespace std;

const char* const kHeaderPrefix = "#";
const char* const kHeaderPrefix2 = "# ";
const auto kLenHeaderPrefix = 1;
const auto kLenHeaderPrefix2 = 2;

string SpecStatefulParser::probeState(const string& line)
{
  auto lstriped = absl::StripLeadingAsciiWhitespace(line);
  prefix_size_ = kLenHeaderPrefix;
  if (status_ == ParserStatus::kStart) {
    if (lstriped.empty()) {
      status_ = ParserStatus::kBody;
    } else if (absl::StartsWith(lstriped, kHeaderPrefix)) {
      status_ = ParserStatus::kHeader;
    } else if (absl::StartsWith(lstriped, kHeaderPrefix2)) {
      status_ = ParserStatus::kHeader;
      prefix_size_ = kLenHeaderPrefix2;
    } else {
      status_ = ParserStatus::kBody;
    }
  } else if (status_ == ParserStatus::kHeader) {
    if (lstriped.empty()) {
      status_ = ParserStatus::kBody;
    } else if (!absl::StartsWith(lstriped, kHeaderPrefix)) {
      status_ = ParserStatus::kBody;
    }
    if (absl::StartsWith(lstriped, kHeaderPrefix2)) {
      prefix_size_ = kLenHeaderPrefix2;
    }
  }
  if (status_ == ParserStatus::kHeader) {
    lstriped.remove_prefix(prefix_size_);
  }
  return string(absl::StripAsciiWhitespace(lstriped));
}

void SpecStatefulParser::Next(string line)
{
  line_num_++;
  absl::StripTrailingAsciiWhitespace(&line);

  auto content = probeState(line);

  switch (status_) {
  case ParserStatus::kHeader:
    spec_->Append({ line, content, nextHeader(content) });
    break;
  case ParserStatus::kBody:
    spec_->Append({ line, content, nextBody(content) });
    break;
  default:
    throw ParsingError(
        fmt::format("invalid internal state at line {}", line_num_));
  }
}

const char kHeaderDelimiter = ':';
const char* const kHeaderVersionKey = "version";
const char* const kHeaderHintsKey = "hints";

unique_ptr<ParsedSpecLine> SpecStatefulParser::nextHeader(string_view content)
{
  if (content.empty()) {
    return nextHeaderLine();
  }
  pair<string, string> p = absl::StrSplit(content, kHeaderDelimiter);

  auto key = absl::StripLeadingAsciiWhitespace(p.first);
  auto value = absl::StripLeadingAsciiWhitespace(p.second);
  auto first_part = string(content.substr(0, content.find(value)));
  if (key == kHeaderVersionKey) {
    return nextHeaderVersion(first_part, value);
  } else if (key == kHeaderHintsKey) {
    return nextHeaderHints(first_part, value);
  } else {
    throw ParsingError(
        fmt::format("invalid key items {} at line {}", key, line_num_));
  }
}

unique_ptr<ParsedSpecLine> SpecStatefulParser::nextHeaderLine()
{
  return make_unique<ParsedSpecLineHeader>();
}

unique_ptr<ParsedSpecLine> SpecStatefulParser::nextHeaderVersion(
    const std::string& key,
    string_view content)
{
  int32_t ver = 1;
  if (content.empty() || !absl::SimpleAtoi(content, &ver)) {
    throw ParsingError(
        fmt::format("parsing header error on version at line ", line_num_));
  }
  if (ver != 1) {
    throw InvalidHeaderError("invalid version. 1 is supported.");
  }

  return make_unique<ParsedSpecVersionHeader>(key, ver);
}

unique_ptr<ParsedSpecLine> SpecStatefulParser::nextHeaderHints(
    const std::string& key,
    string_view content)
{
  if (content.empty()) {
    throw ParsingError(
        fmt::format("parseing header error on hints at line ", line_num_));
  }

  vector<Hint> hints;
  for (auto s : absl::StrSplit(content, gkHeaderHintsDelimiter)) {
    if (s.empty() || s.front() != '[' || s.back() != ']') {
      throw ParsingError(fmt::format("incorrect hints at line {}: {}", line_num_, "an [identifier] is not recoginized."));
    }
    s.remove_prefix(1);
    s.remove_suffix(1);
    pair<string, string> h = absl::StrSplit(s, gkHeaderHintModifierDelimiter);
    HintType type;
    if (h.first == "title") {
      type = HintType::kTitle;
    } else {
      throw InvalidHeaderError(
          fmt::format("invalid hint type {} at line {}", h.first, line_num_));
    }
    HintModifier modifier = HintModifier::kNothing;
    if (h.second == "first word") {
      modifier = HintModifier::kFirstWord;
    }
    hints.push_back({type, modifier});
  }
  auto res = make_unique<ParsedSpecHintsHeader>(key, std::move(hints));
  hints_ = res.get()->hints();
  return res;
}

unique_ptr<ParsedSpecLine> SpecStatefulParser::nextBody(string_view content)
{
  if (content.empty()) {
    return nextBodyLine();
  } else {
    return nextBodyCitation(content);
  }
}

unique_ptr<ParsedSpecLine> SpecStatefulParser::nextBodyLine()
{
  return make_unique<ParsedSpecLineBody>();
}

unique_ptr<ParsedSpecLine> SpecStatefulParser::nextBodyCitation(
    string_view content)
{
  vector<string> parts = absl::StrSplit(content, gkPathDelimiter);
  if (parts.empty() || !absl::StartsWith(parts.front(), "Q")) {
    throw ParsingError(
        absl::StrCat("incorrect citation item at line ", line_num_));
  }

  // TODO: check synced database.
  Citation cite{parts.front(), {}};
  if (hints_ != nullptr) {
    size_t i = 1;
    for (auto iter = hints_->cbegin(); iter != hints_->cend(); ++iter) {
      cite.aux_info.push_back(i < parts.size() ? parts[i] : "");
    }
  }

  return make_unique<ParsedSpecCitationBody>(std::move(cite));
}

}  // namespace wdbib