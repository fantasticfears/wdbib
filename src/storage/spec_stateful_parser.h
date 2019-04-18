#pragma once
#ifndef SPEC_STATEFUL_PARSER_H_
#define SPEC_STATEFUL_PARSER_H_

#include <string>
#include <string_view>
#include <memory>

#include <boost/core/noncopyable.hpp>

namespace wdbib {

class SpecLine;
class SpecFileContent;
class ParsedSpecLine;

class SpecStatefulParser : private boost::noncopyable {
public:
  SpecStatefulParser(const SpecFileContent* content);
  void Next(std::string line);

  std::string_view probeState(const std::string& line);
  std::unique_ptr<ParsedSpecLine> nextHeader(string_view content);
  std::unique_ptr<ParsedSpecLine> nextHeaderVersion(string_view content);
  std::unique_ptr<ParsedSpecLine> nextHeaderHints(string_view content);
  std::unique_ptr<ParsedSpecLine> nextBody(string_view content);
  std::unique_ptr<ParsedSpecLine> nextBodyLine();
  std::unique_ptr<ParsedSpecLine> nextBodyCitation(string_view content);
private:
  enum class ParserStatus {
    kStart,
    kHeader,
    kBody
  } status_;
  SpecFileContent* content_;

  int64_t line_num_ = 0;
  int32_t prefix_size_;
};

}  // namespace wdbib

#endif  // SPEC_STATEFUL_PARSER_H_