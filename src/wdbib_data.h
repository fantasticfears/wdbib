#pragma once
#ifndef WDBIB_DATA_
#define WDBIB_DATA_

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

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
  virtual ~ParsedSpecLine() = default;
};
const char* const kLineHeaderPrefix = "# ";
class ParsedSpecHeader : public ParsedSpecLine
{
 public:
  virtual std::string toString() override = 0;
  virtual void PopulateSpecContent(SpecFileContent* content) override = 0;
  virtual ~ParsedSpecHeader() = default;
};
class ParsedSpecBody : public ParsedSpecLine
{
 public:
  virtual std::string toString() override = 0;
  virtual void PopulateSpecContent(SpecFileContent* content) override = 0;
  virtual ~ParsedSpecBody() = default;
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

// unique_ptr ensures no copy is possible. We use rule of zero for the class
struct SpecLine
{
  std::string data;
  std::string content;
  std::unique_ptr<ParsedSpecLine> parsed;
};

class ParsedSpecVersionHeader : public ParsedSpecHeader
{
 public:
  explicit ParsedSpecVersionHeader(int32_t version) : version_(version) {}
  virtual std::string toString() final;
  virtual void PopulateSpecContent(SpecFileContent* content) final;
  ~ParsedSpecVersionHeader() {};

 private:
  int32_t version_;
};

class ParsedSpecHintsHeader : public ParsedSpecHeader
{
 public:
  explicit ParsedSpecHintsHeader(std::vector<Hint> hints)
      : hints_(std::move(hints))
  {}
  ~ParsedSpecHintsHeader() {};

  virtual std::string toString() final;
  virtual void PopulateSpecContent(SpecFileContent* content) final;
  std::vector<Hint>* hints() { return &hints_; };

 private:
  std::vector<Hint> hints_;
};
class ParsedSpecLineHeader : public ParsedSpecHeader
{
 public:
  virtual std::string toString() final { return {}; }
  virtual void PopulateSpecContent(SpecFileContent*) final {}
  ~ParsedSpecLineHeader() {};
};

class ParsedSpecLineBody : public ParsedSpecBody
{
 public:
  virtual std::string toString() override { return {}; }
  virtual void PopulateSpecContent(SpecFileContent*) override {}
  ~ParsedSpecLineBody() {};
};

class ParsedSpecCitationBody : public ParsedSpecBody
{
 public:
  explicit ParsedSpecCitationBody(Citation item) : item_(std::move(item)) {}
  virtual std::string toString() final;
  virtual void PopulateSpecContent(SpecFileContent* content) final;
  ~ParsedSpecCitationBody() {};
  Citation& item() { return item_; }

 private:
  Citation item_;
};

SpecLine GetSpecLineFromCitation(const Citation& item);

class SpecFileContent : private boost::noncopyable
{
 public:
  explicit SpecFileContent() {}
  size_t Size() const { return spec_lines_.size(); }
  void Append(SpecLine&& line, bool update = true);
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
  std::vector<SpecLine> spec_lines_;
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
  nlohmann::json Find(const std::string& qid) { return data_.at(qid); }
  bool Found(const std::string& qid) { return data_.find(qid) != data_.end(); }
  std::unordered_map<std::string, nlohmann::json>& All() { return data_; }

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