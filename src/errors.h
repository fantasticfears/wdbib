#pragma once
#ifndef ERRORS_H_
#define ERRORS_H_

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

class WikidataParsingError : public std::runtime_error
{
 public:
  explicit WikidataParsingError(const std::string& what) : std::runtime_error(what) {}
  explicit WikidataParsingError(const char* what) : std::runtime_error(what) {}
};

class InvalidWikiciteItemError : public std::runtime_error
{
 public:
  explicit InvalidWikiciteItemError(const std::string& what) : std::runtime_error(what) {}
  explicit InvalidWikiciteItemError(const char* what) : std::runtime_error(what) {}
};

}  // namespace wdbib

#endif  // ERRORS_H_
