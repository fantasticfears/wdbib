#include "wdbib_data.h"

namespace wdbib {

void SpecFileContent::appendCitation(const string& qid)
{
  unordered_map[qid] = spec_lines.size() - 1;
}

void SpecFileContent::Append(const SpecLine& line)
{
  spec_lines_.push_back(line);
  lines_removed_.push_back(false);
  auto parsed = *spec_lines.back().parsed;
  parsed->PopulateSpecContent(this);
}

std::optional<SpecLine*> SpecFileContent::Line(size_t line) const
{
  if (line < spec_lines_.size() && !lines_removed_[line]) {
    return &spec_lines[line];
  } else {
    return {};
  }
}

void SpecFileContent::RemoveLine(size_t line)
{
  if (line < lines_removed_.size()) {
    lines_removed_[line] = true;
  }
}

void DataFileContent::Load(const nlohmann::json& data)
{
  data_ = data;
}

string DataFileContent::Dump() const
{
  return data_.dump();
}


}  // namespace wdbib
