#pragma once
#ifndef WDBIB_DATA_
#define WDBIB_DATA_

#include <vector>

#include <boost/core/noncopyable.hpp>
#include <nlohmann/json.hpp>

#include "../storage/data_file.h"

namespace wdbib {

class SpecFileContent : private boost::noncopyable {
public:
  explicit SpecFileContent() {}
  void Append(const SpecLine& line);
  size_t Size() const;
  std::string Line(size_t line) const;
private:
  std::vector<SpecLine> spec_lines_;
};

class DataFileContent : private boost::noncopyable  {
public:
  explicit DataFileContent() {}
  void Load(const nlohmann::json& data);
  nlohmann::json Dump() const;
private:
  nlohmann::json data_;
}

struct WdbibFileContent {
  SpecFileContent spec;
  DataFileContent data;
};


}  // namespace wdbib

#endif  // WDBIB_DATA_