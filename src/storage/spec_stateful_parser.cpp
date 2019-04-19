#include "spec_stateful_parser.h"

#include <string>

#include <absl/strings/ascii.h>
#include <absl/strings/match.h>
#include <absl/strings/numbers.h>
#include <absl/strings/str_cat.h>
#include <absl/strings/str_split.h>
#include <fmt/format.h>

#include "../errors.h"
#include "../wdbib_data.h"
#include "data_file.h"

namespace wdbib {

using namespace std;

const char* const kHeaderPrefix = "#";
const char* const kHeaderPrefix2 = "# ";
const auto kLenHeaderPrefix = 1;
const auto kLenHeaderPrefix2 = 2;

string_view SpecStatefulParser::probeState(const string& line)
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
  return absl::StripAsciiWhitespace(lstriped);
}

void SpecStatefulParser::Next(string line)
{
  line_num_++;
  absl::StripTrailingAsciiWhitespace(&line);

  auto content = probeState(line);
  switch (status_) {
  case ParserStatus::kHeader:
    spec_->Append(make_unique<SpecLine>(line, content, nextHeader(content)));
    break;
  case ParserStatus::kBody:
    spec_->Append(make_unique<SpecLine>(line, content, nextBody(content)));
    break;
  default:
    throw ParsingError(fmt::format("invalid internal state at line {}", line_num_));
  }
}

const char kHeaderDelimiter = ':'; 
const char* const kHeaderVersionKey = "version";
const char* const kHeaderHintsKey = "hints";

unique_ptr<ParsedSpecLine> SpecStatefulParser::nextHeader(string_view content)
{
  pair<string, string> p = absl::StrSplit(content, kHeaderDelimiter);

  auto key = absl::StripLeadingAsciiWhitespace(p.first);
  auto value = absl::StripLeadingAsciiWhitespace(p.second);
  if (key == kHeaderVersionKey) {
    return nextHeaderVersion(value);
  } else if (key == kHeaderHintsKey) {
    return nextHeaderHints(value);
  } else {
    throw ParsingError(fmt::format("invalid key items {} at line {}", key, line_num_));
  }
}

unique_ptr<ParsedSpecLine> SpecStatefulParser::nextHeaderVersion(string_view content)
{
  int32_t ver = 1;
  if (content.empty() || !absl::SimpleAtoi(content, &ver)) {
    throw ParsingError(
        fmt::format("parsing header error on version at line ", line_num_));
  }
  if (ver != 1) {
    throw InvalidSpecError("invalid version. 1 is supported.");
  }
  
  return make_unique<ParsedSpecVersionHeader>(ver);
}

unique_ptr<ParsedSpecLine> SpecStatefulParser::nextHeaderHints(string_view content)
{
  if (content.empty()) {
    throw ParsingError(fmt::format("parseing header error on hints at line ", line_num_));
  }
  
  vector<Hint> hints;
  for (auto s : absl::StrSplit(content, gkHeaderHintsDelimiter)) {
    if (s.empty() || s.front() != '[' || s.back() != ']') {
      throw ParsingError(absl::StrCat("incorrect hints at line ", line_num_));
    }
    s.remove_prefix(1);
    s.remove_suffix(1);
    pair<string, string> h = absl::StrSplit(s, gkHeaderHintModifierDelimiter);
    HintType type;
    if (h.first == "article") {
      type = HintType::kArticle;
    } else {
      throw InvalidSpecError(fmt::format("invalid hint type {} at line {}", h.first, line_num_));
    }
    HintModifier modifier = HintModifier::kNothing;
    if (h.second == "first word") {
      modifier = HintModifier::kFirstWord;
    }
    hints.push_back({ type, modifier });
  }
  auto res = make_unique<ParsedSpecHintsHeader>(std::move(hints));
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


unique_ptr<ParsedSpecLine> SpecStatefulParser::nextBodyCitation(string_view content)
{
  vector<string> parts = absl::StrSplit(content, gkPathDelimiter);
  if (parts.empty() || !absl::StartsWith(parts.front(), "Q")) {
    throw ParsingError(
        absl::StrCat("incorrect citation item at line ", line_num_));
  }

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