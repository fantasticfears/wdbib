#pragma once
#ifndef WDBIB_DATA_
#define WDBIB_DATA_

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include <boost/core/noncopyable.hpp>
#include <nlohmann/json.hpp>

#include "storage/data_file.h"

namespace wdbib {

class SpecFileContent;

const char* const gkPathDelimiter = ":";
const char* const gkHeaderHintsDelimiter = "|";
const char* const gkHeaderHintModifierDelimiter = "/";

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

class SpecFileContent : private boost::noncopyable
{
 public:
  explicit SpecFileContent() {}
  size_t Size() const { return spec_lines_.size(); }
  void Append(std::unique_ptr<SpecLine> line);
  std::optional<const SpecLine*> Line(size_t line) const;
  void RemoveLine(size_t line);
  bool Found(const std::string& qid) const;
  ParsedSpecCitationBody* Find(const std::string& qid) const;
  std::vector<std::string> QIDs() const;
  std::string Dump() const;

  int32_t* version() const { return version_; }
  void set_version(int32_t* version) { version_ = version; }
  std::vector<Hint>* hints() const { return hints_; }
  void set_hints(std::vector<Hint>* hints) { hints_ = hints; }
  void appendCitation(const std::string& qid);
  bool updated() const { return updated_; }
  void set_updated(bool updated = true) { updated_ = updated; }

 private:
  std::vector<std::unique_ptr<SpecLine>> spec_lines_;
  std::vector<bool> lines_removed_;
  std::unordered_map<std::string, size_t> loaded_citation_;
  int32_t* version_ = nullptr;
  std::vector<Hint>* hints_ = nullptr;
  bool updated_;
};

class DataFileContent : private boost::noncopyable
{
 public:
  explicit DataFileContent() {}
  void Load(const nlohmann::json& data);
  std::string Dump() const;
  void Update(const nlohmann::json& resp);
  void Remove(const std::string& qid);
  bool Found(const std::string& qid) { return data_.find(qid) != data_.end(); }
  std::unordered_map<std::string, nlohmann::json>& All() 
  {
    return data_;
  }

  // nlohmann::json& data() { return data_; }
  bool updated() const { return updated_; }
  void set_updated(bool updated = true) { updated_ = updated; }

 private:
  unordered_map<std::string, nlohmann::json> data_;
  bool updated_;
};

struct WdbibFileContent
{
  SpecFileContent spec;
  DataFileContent data;
};

}  // namespace wdbib

#endif  // WDBIB_DATA_