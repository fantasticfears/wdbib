#pragma once
#ifndef SPEC_STATEFUL_PARSER_H_
#define SPEC_STATEFUL_PARSER_H_

#include <string>
#include <string_view>
#include <memory>
#include <vector>

#include <boost/core/noncopyable.hpp>

namespace wdbib {

struct SpecLine;
class SpecFileContent;
class ParsedSpecLine;
struct Hint;

const char* const gkPathDelimiter = ":";
const char* const gkHeaderHintsDelimiter = "|"; 
const char* const gkHeaderHintModifierDelimiter = "/"; 

class SpecStatefulParser : private boost::noncopyable {
public:
  explicit SpecStatefulParser(SpecFileContent* spec): spec_(spec) {}
  void Next(std::string line);

  std::string_view probeState(const std::string& line);
  std::unique_ptr<ParsedSpecLine> nextHeader(std::string_view content);
  std::unique_ptr<ParsedSpecLine> nextHeaderVersion(std::string_view content);
  std::unique_ptr<ParsedSpecLine> nextHeaderHints(std::string_view content);
  std::unique_ptr<ParsedSpecLine> nextBody(std::string_view content);
  std::unique_ptr<ParsedSpecLine> nextBodyLine();
  std::unique_ptr<ParsedSpecLine> nextBodyCitation(std::string_view content);
private:
  enum class ParserStatus {
    kStart,
    kHeader,
    kBody
  } status_ = ParserStatus::kStart;

  SpecFileContent* spec_;
  int64_t line_num_ = 0;
  int32_t prefix_size_;
  std::vector<Hint>* hints_ = nullptr;
};

}  // namespace wdbib

#endif  // SPEC_STATEFUL_PARSER_H_