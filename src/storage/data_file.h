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
#include <boost/move/utility_core.hpp>

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
  void LoadAll(std::function<void(std::ifstream&)> spec,
               std::function<void(std::ifstream&)> data) const;
  void LoadSpec(std::function<void(std::ifstream&)> spec) const;
  void LoadData(std::function<void(std::ifstream&)> data) const;
  void SaveAll(std::function<void(std::ofstream&)> spec,
               std::function<void(std::ofstream&)> data) const;
  void SaveSpec(std::function<void(std::ofstream&)> spec) const;
  void SaveData(std::function<void(std::ofstream&)> data) const;

 private:
  std::string spec_filename_;
  std::string data_filename_;
};

class SpecFileContent;
class ParsedSpecLine : private boost::noncopyable
{
 public:
  virtual std::string toString() = 0;
  virtual void PopulateSpecContent(SpecFileContent* content) = 0;
  virtual ~ParsedSpecLine(){};
};
const char* const kLineHeaderPrefix = "# ";
class ParsedSpecHeader : public ParsedSpecLine
{
 public:
  virtual std::string toString() override = 0;
  virtual void PopulateSpecContent(SpecFileContent* content) override = 0;
};
class ParsedSpecBody : public ParsedSpecLine
{
 public:
  virtual std::string toString() override = 0;
  virtual void PopulateSpecContent(SpecFileContent* content) override = 0;
};

struct Citation
{
  std::string qid;
  std::vector<std::string> aux_info;
};

enum class HintType
{
  kTitle
};

enum class HintModifier
{
  kNothing,
  kFirstWord
};

struct Hint
{
  HintType type;
  HintModifier modifier;
};

struct SpecLine
{
 public:
  std::string data;
  std::string_view content;
  std::unique_ptr<ParsedSpecLine> parsed;
  SpecLine(std::string d, std::string_view c, std::unique_ptr<ParsedSpecLine> p)
      : data(std::move(d)), content(std::move(c)), parsed(std::move(p))
  {}

 private:
  BOOST_MOVABLE_BUT_NOT_COPYABLE(SpecLine)
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
  explicit ParsedSpecHintsHeader(std::vector<Hint> hints)
      : hints_(std::move(hints))
  {}
  virtual std::string toString() override;
  virtual void PopulateSpecContent(SpecFileContent* content) override;
  std::vector<Hint>* hints() { return &hints_; };

 private:
  std::vector<Hint> hints_;
};
class ParsedSpecLineHeader : public ParsedSpecHeader
{
 public:
  virtual std::string toString() override { return {}; }
  virtual void PopulateSpecContent(SpecFileContent*) override {}
};

class ParsedSpecLineBody : public ParsedSpecBody
{
 public:
  virtual std::string toString() override { return {}; }
  virtual void PopulateSpecContent(SpecFileContent*) override {}
};

class ParsedSpecCitationBody : public ParsedSpecBody
{
 public:
  explicit ParsedSpecCitationBody(Citation item) : item_(std::move(item)) {}
  virtual std::string toString() override;
  virtual void PopulateSpecContent(SpecFileContent* content) override;
  Citation& item() { return item_; }

 private:
  Citation item_;
};

std::unique_ptr<SpecLine> MakeSpecLine(const Citation& item);

struct WdbibFileContent;
class DataFileContent;

namespace file {

std::unique_ptr<WdbibFileContent> LoadWdbibData(const BibDataFile& file);
void SaveWdbibData(const BibDataFile& file, const WdbibFileContent* content);

}  // namespace file

}  // namespace wdbib

#endif  // DATA_FILE_H_
