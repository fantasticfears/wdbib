#include "data_file.h"

#include <fstream>
#include <iostream>
#include <utility>

#include <absl/strings/ascii.h>
#include <absl/strings/match.h>
#include <absl/strings/numbers.h>
#include <absl/strings/str_cat.h>
#include <absl/strings/str_split.h>

#include "../errors.h"

namespace wdbib {

using namespace std;

namespace file {

const int kFileEndOffset = 0;
const int kFileStartOffset = 0;

string ReadAll(const string& path)
{
  ifstream fs(path);
  fs.seekg(kFileEndOffset, ios::end);
  auto size = fs.tellg();
  string buffer(size, ' ');
  fs.seekg(kFileStartOffset);
  fs.read(&buffer[0], size);
  return buffer;
}

void OverWrite(const string& path, const string& content)
{
  ofstream fs(path, ofstream::out | ofstream::trunc);
  fs.write(content.c_str(), content.size());
}

void Write(const string& path, const string& content)
{
  ofstream fs(path, ofstream::out);
  fs.write(content.c_str(), content.size());
}

}  // namespace file

const char* const kHeaderPrefix = "# ";
const char* const kHeaderSpecKey = "spec";
const char* const kHeaderHintsKey = "hints";
const char kPathSpecDelimiter = ':';
const auto kNumExtraCharAfterKey = 1;

Spec TryParseSpecWithDefault(string_view spec_str, const string& default_str,
                             int32_t line_num)
{
  if (spec_str.empty()) {
    spec_str = default_str;
  }
  spec_str = absl::StripAsciiWhitespace(spec_str);
  int32_t num = 1;
  if (spec_str.empty() || !absl::SimpleAtoi(spec_str, &num)) {
    throw ParsingError(
        absl::StrCat("incorrect spec version at line ", line_num));
  }
  if (num != 1) {
    throw InvalidSpecError("invalid spec version. 1 is supported.");
  }
  return string(spec_str);
}

CitationHints TryParseHintsWithDefault(string_view hints_str,
                                       const string& default_str,
                                       int32_t line_num)
{
  if (hints_str.empty()) {
    hints_str = default_str;
  }
  CitationHints hints;
  for (auto s : absl::StrSplit(absl::StripAsciiWhitespace(hints_str), ':')) {
    if (s.empty() || s.front() != '[' || s.back() != ']') {
      throw ParsingError(absl::StrCat("incorrect hints at line ", line_num));
    }
    s.remove_prefix(1);
    s.remove_suffix(1);
    hints.push_back(string(s));
  }

  return hints;
}

optional<Citation> ParseCitation(const CitationHints& hints,
                                 string_view cite_str, int32_t line_num)
{
  if (cite_str.empty()) {
    return {};
  }

  vector<string> parts = absl::StrSplit(cite_str, kPathSpecDelimiter);
  if (parts.size() < 2 || parts[0] != "wc" ||
      !absl::StartsWith(parts[1], "Q")) {
    throw ParsingError(
        absl::StrCat("incorrect citation item at line ", line_num));
  }

  Citation cite{parts[1]};
  int i = 2;
  for (const auto& com : hints) {
    if (i < parts.size()) {
      cite.aux_info.push_back(parts[i]);
    } else {
      cite.aux_info.push_back("");
    }
  }
  return {cite};
}

BibDataFile::BibDataFile(const string& path) : path_(path) {}

BibDataFileContent BibDataFile::Parse() const
{
  ifstream fin(path_);
  string line;
  int32_t line_num = 0;
  BibDataFileContent bib;
  bool break_header = false;
  while (getline(fin, line)) {
    line_num++;
    if (absl::StartsWith(line, kHeaderPrefix)) {
      pair<string, string> p = absl::StrSplit(line, '#');
      auto s = absl::StripLeadingAsciiWhitespace(p.second);
      if (absl::StartsWith(s, kHeaderSpecKey)) {
        s.remove_prefix(strlen(kHeaderSpecKey) + kNumExtraCharAfterKey);
        bib.spec = TryParseSpecWithDefault(s, content::kDefaultSpec, line_num);
      } else if (absl::StartsWith(s, "hints")) {
        s.remove_prefix(strlen(kHeaderHintsKey) + kNumExtraCharAfterKey);
        bib.path_spec =
            TryParseHintsWithDefault(s, content::kDefaultHints, line_num);
      }

      bib.headers.push_back(line);
    } else {
      break_header = true;
      break;
    }
  }

  if (break_header) {
    if (auto cite = ParseCitation(bib.path_spec, line, line_num); cite) {
      bib.items.push_back(*cite);
    }
  }
  while (getline(fin, line)) {
    line_num++;
    if (auto cite = ParseCitation(bib.path_spec, line, line_num); cite) {
      bib.items.push_back(*cite);
    }
  }
  return bib;
}

}  // namespace wdbib
