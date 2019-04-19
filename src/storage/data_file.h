#pragma once
#ifndef DATA_FILE_H_
#define DATA_FILE_H_

#include <fstream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <boost/core/noncopyable.hpp>

#include "../wikicite.h"

namespace wdbib {

// kDefaultDataFilename is the filename for storing uris
const char* const kDefaultDataFilename = "citation";

// kDefaultCachedDataFilename is the filename for storing cached wikidata
// information
const char* const kDefaultCachedDataFilename = "citation.lock";
const char* const kDefaultCachedDataExtrension = "lock";

/*
 * BibDataFile manages the data file and exposes streams to the user.
 */
class BibDataFile : private boost::noncopyable
{
 public:
  BibDataFile(const std::string& filename, const std::string& lock_ext);
  void Load(function<void(std::ifstream&)> spec,
            function<void(std::ifstream&)> data);
  void SaveAll(function<void(std::ofstream&)> spec,
               function<void(std::ofstream&)> data);
  void SaveSpec(function<void(std::ofstream&)> spec);
  void SaveData(function<void(std::ofstream&)> data);

 private:
  std::string spec_filename_;
  std::string data_filename_;
};

struct ParsedSpecLine : private boost::noncopyable
{
  virtual std::string toString() = 0;
  virtual void PopulateSpecContent(SpecFileContent* content) = 0;
};
const char* const kLineHeaderPrefix = "# ";
struct ParsedSpecHeader : public ParsedSpecLine
{
  virtual std::string toString() override = 0;
  virtual void PopulateSpecContent(SpecFileContent* content) = 0;
};
struct ParsedSpecBody : public ParsedSpecLine
{
  virtual std::string toString() override = 0;
  virtual void PopulateSpecContent(SpecFileContent* content) = 0;
};

struct Citation
{
  string qid;
  vector<string> aux_info;
};

enum class HintType
{
  kArticle
};

enum class HintModifier
{
  kFirstWord
};

struct Hints
{
  HintType type;
  HintModifier modifier;
};

struct SpecLine : private boost::noncopyable
{
  std::string data;
  std::string_view content;
  std::unique_ptr<ParsedSpecLine> parsed;
};

class ParsedSpecVersionHeader : public ParsedSpecHeader
{
 public:
  explicit ParsedSpecVersionHeader(int32_t version) : version_(version) {}
  virtual std::string toString() override;
  virtual void PopulateSpecContent(SpecFileContent* content) override;

 private:
  int32_t version_;
};

class ParsedSpecHintsHeader : public ParsedSpecHeader
{
 public:
  explicit ParsedSpecHintsHeader(const std::vector<Hints>& hints)
      : hints_(hints)
  {}
  explicit ParsedSpecHintsHeader(std::vector<Hints> hints)
      : hints_(std::move(hints))
  {}
  virtual std::string toString() override;
  virtual void PopulateSpecContent(SpecFileContent* content) override;

 private:
  std::vector<Hints> hints_;
};

class ParsedSpecLineBody : public ParsedSpecBody
{
 public:
  virtual std::string toString() override { return {}; }
  virtual void PopulateSpecContent(SpecFileContent* content) override {}

}

class ParsedSpecCitationBody : public ParsedSpecBody
{
 public:
  explicit ParsedSpecCitationBody(const Citation& item) : item_(item) {}
  explicit ParsedSpecCitationBody(Citation item) : item_(std::move(item)) {}
  virtual std::string toString() override;
  virtual void PopulateSpecContent(SpecFileContent* content) override;

 private:
  Citation item_;
};

class WdbibFileContent;

namespace file {

WdbibFileContent LoadWdbibData(const BibDataFile& file);
void SaveWdbibData(const BibDataFile& file, const WdbibFileContent& content);

}  // namespace file

}  // namespace wdbib

#endif  // DATA_FILE_H_
