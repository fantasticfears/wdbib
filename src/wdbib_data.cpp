#include "wdbib_data.h"

namespace wdbib {

void SpecFileContent::appendCitation(const string& qid)
{
  auto ln = spec_lines_.size() - 1;
  loaded_citation_[qid] = ln;
}

void SpecFileContent::Append(std::unique_ptr<SpecLine> line)
{
  spec_lines_.push_back(std::move(line));
  lines_removed_.push_back(false);
  spec_lines_.back()->parsed->PopulateSpecContent(this);
  set_updated(true);
}

bool SpecFileContent::Found(const string& qid) const
{
  auto m = loaded_citation_.find(qid);

  return m != loaded_citation_.end() && !lines_removed_[m->second];
}

ParsedSpecCitationBody* SpecFileContent::Find(const string& qid) const
{
  if (!Found(qid)) {
    throw std::runtime_error("can't find qid.");
  }
  return dynamic_cast<ParsedSpecCitationBody*>(
      spec_lines_[loaded_citation_.find(qid)->second]->parsed.get());
}

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
    ParsedSpecCitationBody* b =
        dynamic_cast<ParsedSpecCitationBody*>(spec_lines_[idx]->parsed.get());
    res.push_back(b->item().qid);
  }
  return res;
}

std::optional<const SpecLine*> SpecFileContent::Line(size_t line) const
{
  if (line < spec_lines_.size() && !lines_removed_[line]) {
    return {const_cast<const SpecLine*>(spec_lines_[line].get())};
  } else {
    return {};
  }
}

void SpecFileContent::RemoveLine(size_t line)
{
  if (line < lines_removed_.size()) {
    lines_removed_[line] = true;
    set_updated(true);
  }
}

// FIXME: UPDATE
void DataFileContent::Update(const nlohmann::json& data)
{
  data_ = data;
  set_updated(true);
}

void DataFileContent::Load(const nlohmann::json& data)
{
  data_ = data;
  set_updated(true);
}

string DataFileContent::Dump() const { return data_.dump(); }

}  // namespace wdbib
