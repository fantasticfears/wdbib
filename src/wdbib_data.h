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
  bool Found(const std::string& qid) { return data_.find(qid) != data_.end(); }
  std::unordered_map<std::string, nlohmann::json> All() const
  {
    std::unordered_map<std::string, nlohmann::json> res;
    for (auto& [key, value] : data_.items()) {
      res[key] = value;
    }
    return res;
  }

  nlohmann::json& data() { return data_; }
  bool updated() const { return updated_; }
  void set_updated(bool updated = true) { updated_ = updated; }

 private:
  nlohmann::json data_;
  bool updated_;
};

struct WdbibFileContent
{
  SpecFileContent spec;
  DataFileContent data;
};

}  // namespace wdbib

#endif  // WDBIB_DATA_