#pragma once

#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>
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

const char* const kDefaultHints = "";

const char* const kDefaultSpec = "1";

const char* const kDefaultHeader = "# spec: 1\n";

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
  CitationHints path_spec;

  vector<Citation> items;

  vector<string> headers;
  vector<string> text;
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
  void Save(const BibDataFileContent& content) const;

 private:
  string path_;
};

using nlohmann::json;

class BibDataLockFile
{
 public:
  explicit BibDataLockFile(const string& path) : path_(path)
  {
    std::ifstream i(path_);
    try {
    data_ = json::parse(i);
    } catch (...) {
      data_ = json();
    }
  }
  void update(const string& resp)
  {
    auto r = json::parse(resp);
    for (auto& [key, value] : r.at("entities").items()) {
      data_[key] = value;
    }
  }
  void Save()
  {
    std::ofstream f(path_, std::ios::out | std::ios::trunc);
    f << data_.dump() << std::endl;
  }

 private:
  string path_;
  json data_;
};

}  // namespace wdbib
