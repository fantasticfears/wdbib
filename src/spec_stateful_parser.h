#pragma once
#ifndef SPEC_STATEFUL_PARSER_H_
#define SPEC_STATEFUL_PARSER_H_

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <boost/core/noncopyable.hpp>

namespace wdbib {

struct SpecLine;
class SpecFileContent;
class ParsedSpecLine;
struct Hint;

enum class ParserStatus
{
  kStart,
  kHeader,
  kBody
};

class SpecStatefulParser : private boost::noncopyable
{
 public:
  explicit SpecStatefulParser(SpecFileContent* spec) : spec_(spec)
  {
    assert(spec != nullptr);
  }
  void Next(std::string line);
  ParserStatus status() const { return status_; };

  std::string probeState(const std::string& line);
  std::unique_ptr<ParsedSpecLine> nextHeader(std::string_view content);
  std::unique_ptr<ParsedSpecLine> nextHeaderLine();
  std::unique_ptr<ParsedSpecLine> nextHeaderVersion(const std::string& key, std::string_view content);
  std::unique_ptr<ParsedSpecLine> nextHeaderHints(const std::string& key, std::string_view content);
  std::unique_ptr<ParsedSpecLine> nextBody(std::string_view content);
  std::unique_ptr<ParsedSpecLine> nextBodyLine();
  std::unique_ptr<ParsedSpecLine> nextBodyCitation(std::string_view content);

 private:
  ParserStatus status_ = ParserStatus::kBody;

  SpecFileContent* spec_;
  int64_t line_num_ = 0;
  int32_t prefix_size_;
  std::vector<Hint>* hints_ = nullptr;
};

}  // namespace wdbib

#endif  // SPEC_STATEFUL_PARSER_H_