#pragma once

#include <stdexcept>
#include <string>

namespace wdbib {

class ParsingError : public std::runtime_error
{
 public:
  explicit ParsingError(const std::string& what) : std::runtime_error(what) {}
  explicit ParsingError(const char* what) : std::runtime_error(what) {}
};

class InvalidSpecError : public std::runtime_error
{
 public:
  explicit InvalidSpecError(const std::string& what) : std::runtime_error(what) {}
  explicit InvalidSpecError(const char* what) : std::runtime_error(what) {}
};

}  // namespace wdbib
