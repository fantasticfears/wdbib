#pragma once

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace wdbib {

using std::optional;
using std::string;
using std::string_view;
using std::vector;

namespace content {

const char* const kDefaultHints = "[title]";

const char* const kDefaultSpec = "1";

const char* const kDefaultHeader =
    "# spec: 1\n"
    "# hints: [title]\n\n";

}  // namespace content

namespace file {

string ReadAll(const string& path);
void OverWrite(const string& path, const string& content);
void Write(const string& path, const string& content);

}  // namespace file

struct Citation
{
  string qid;
  vector<string> aux_info;
};

typedef vector<string> CitationHints;

struct BibDataFileContent
{
  string spec;
  vector<string> headers;
  CitationHints path_spec;
  vector<Citation> items;
};

typedef string Spec;

Spec TryParseSpecWithDefault(string_view spec_str, const string& default_str,
                             int32_t line_num);
CitationHints TryParseHintsWithDefault(string_view hints_str,
                                       const string& default_str,
                                       int32_t line_num);
optional<Citation> ParseCitation(const CitationHints& hints,
                                 string_view cite_str, int32_t line_num);

/*
 * BibDataFile manages the data file.
 * It's aware of semantics of file.
 */
class BibDataFile
{
 public:
  explicit BibDataFile(const string& path);
  BibDataFileContent Parse() const;

 private:
  string path_;
};

}  // namespace wdbib
