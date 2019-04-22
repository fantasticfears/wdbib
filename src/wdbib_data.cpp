#include "wdbib_data.h"

#include <absl/strings/str_cat.h>
#include <absl/strings/str_join.h>
#include "spdlog/spdlog.h"

#include "errors.h"

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

std::string SpecFileContent::Dump() const
{
  string res;
  for (size_t i = 1; i <= Size(); ++i) {
    if (auto s = Line(i); s) {
      const auto& parsed = (*s)->parsed;
      const auto& data = (*s)->data;
      const auto& content = (*s)->content;

      auto rendered = parsed->toString();
      if (rendered == content) {
        absl::StrAppend(&res, data, "\n");
      } else {
        absl::StrAppend(&res, data.substr(data.find(content)), rendered, "\n");
        spdlog::get("stderr")->debug("dump spec: %s\n", res);
        spdlog::get("stderr")->debug("content stored: %s\n", content);
      }
    }
  }
  return res;
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
  line--;
  if (line < spec_lines_.size() && !lines_removed_[line]) {
    return {const_cast<const SpecLine*>(spec_lines_[line].get())};
  } else {
    return {};
  }
}

void SpecFileContent::RemoveLine(size_t line)
{
  line--;
  if (line < lines_removed_.size()) {
    lines_removed_[line] = true;
    set_updated(true);
  }
}

void DataFileContent::Update(const nlohmann::json& data)
{
  try {
    auto id = data.get<wd::WikidataItem>().id;
    auto p = data_.find(id);
    if (p == data_.end()) {
      data_.insert({id, data});
    } else if (data != p->second) {
      p->second = data;
      set_updated(true);
    }
  } catch (...) {
    throw WikidataParsingError(
        "internal state update failure: illformat wikidata json tries to be "
        "added into db");
  }
}

/*
 * For database loading
 */
void DataFileContent::Load(const nlohmann::json& data)
{
  for (auto& el : data.items()) {
    data_.insert({el.key(), json(el.value())});
  }
}

void DataFileContent::Remove(const std::string& qid) { data_.erase(qid); }

string DataFileContent::Dump() const
{
  string serialized = "{";

  for (const auto& [k, v] : data_) {
    absl::StrAppend(&serialized, "\"", k, "\":", v.get<std::string>());
  }
  absl::StrAppend(&serialized, "}");
  return serialized;
}

unique_ptr<SpecLine> MakeSpecLine(const Citation& item)
{
  return make_unique<SpecLine>("", "",
                               make_unique<ParsedSpecCitationBody>(item));
}

string ParsedSpecVersionHeader::toString() { return absl::StrCat(version_); }

// extern const char* const kHeaderHintModifierDelimiter;
// const char* const gkPathDelimiter = ":";
// const char* const gkHeaderHintsDelimiter = "|";
// extern const char* const gkHeaderHintModifierDelimiter = "/";
struct HintsFormatter
{
  void operator()(std::string* out, const Hint& hint) const
  {
    switch (hint.type) {
    case HintType::kTitle:
      absl::StrAppend(out, "title");
      break;
    default:
      break;
    }
    switch (hint.modifier) {
    case HintModifier::kFirstWord:
      absl::StrAppend(out, gkHeaderHintModifierDelimiter, "first word");
      break;

    default:
      break;
    }
  }
};

extern const char* const gkHeaderHintsDelimiter;
string ParsedSpecHintsHeader::toString()
{
  return absl::StrJoin(hints_, gkHeaderHintsDelimiter, HintsFormatter());
}

extern const char* const gkPathDelimiter;

string ParsedSpecCitationBody::toString()
{
  if (!item_.aux_info.empty()) {
    return absl::StrCat(item_.qid, gkPathDelimiter,
                        absl::StrJoin(item_.aux_info, gkPathDelimiter));
  } else {
    return item_.qid;
  }
}

void ParsedSpecVersionHeader::PopulateSpecContent(SpecFileContent* content)
{
  content->set_version(&version_);
}

void ParsedSpecHintsHeader::PopulateSpecContent(SpecFileContent* content)
{
  content->set_hints(&hints_);
}

void ParsedSpecCitationBody::PopulateSpecContent(SpecFileContent* content)
{
  content->appendCitation(item_.qid);
}

}  // namespace wdbib
