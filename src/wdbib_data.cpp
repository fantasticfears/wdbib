#include "wdbib_data.h"

namespace wdbib {

void SpecFileContent::appendCitation(const string& qid)
{
  auto ln = spec_lines.size() - 1;
  unordered_map[qid] = ln;
  citation_line_.push_back(ln);
}

void SpecFileContent::Append(const SpecLine& line)
{
  spec_lines_.push_back(line);
  lines_removed_.push_back(false);
  auto parsed = *spec_lines.back().parsed;
  parsed->PopulateSpecContent(this);
}

bool SpecFileContent::Found(const string& qid) const { return loaded_citation_.find(qid) != loaded_citation_.end(); };

std::vector<std::string> SpecFileContent::QIDs() const
{
  vector<size_t> qid_spec_line_idx;

  for (const auto& [qid, idx] : loaded_citation_) {
    if (!lines_removed_[idx]) {
      qid_spec_line_idx.push_back(idx);
    }
  }
  
  vector<string> res;
  for (auto idx : qid_spec_line_idx) {
    ParsedSpecCitationBody* b = spec_lines_[idx].parsed;
    res.push_back(b->qid);
  }
  return res;
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
