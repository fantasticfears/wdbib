#include "data_file.h"

#include <fstream>
#include <iostream>
#include <utility>
#include <memory>

#include <absl/strings/str_cat.h>
#include <absl/strings/str_join.h>

#include "../wdbib_data.h"
#include "spec_stateful_parser.h"

namespace wdbib {

using namespace std;

BibDataFile::BibDataFile(const string& filename, const string& lock_ext)
    : spec_filename_(filename),
      data_filename_(absl::StrCat(filename, ".", lock_ext))
{}

void BibDataFile::LoadAll(function<void(ifstream&)> spec,
                          function<void(ifstream&)> data) const
{
  LoadSpec(spec);
  LoadData(data);
}

void BibDataFile::LoadSpec(function<void(std::ifstream&)> spec) const
{
  ifstream sf(spec_filename_);
  spec(sf);
}
void BibDataFile::LoadData(function<void(std::ifstream&)> data) const
{
  ifstream df(data_filename_);
  data(df);
}

void BibDataFile::SaveAll(function<void(ofstream&)> spec,
                          function<void(ofstream&)> data) const
{
  SaveSpec(spec);
  SaveData(data);
}

void BibDataFile::SaveSpec(function<void(std::ofstream&)> spec) const
{
  ofstream sf(spec_filename_, std::ios::out | std::ios::trunc);
  spec(sf);
}

void BibDataFile::SaveData(function<void(std::ofstream&)> data) const
{
  ofstream df(data_filename_, std::ios::out | std::ios::trunc);
  data(df);
}

unique_ptr<SpecLine> MakeSpecLine(const Citation& item)
{
  return make_unique<SpecLine>("", "", make_unique<ParsedSpecCitationBody>(item));
}

namespace file {

unique_ptr<WdbibFileContent> LoadWdbibData(const BibDataFile& file)
{
  auto content = make_unique<WdbibFileContent>();

  SpecStatefulParser spec_parser(&(content->spec));
  file.LoadAll(
      [&](ifstream& f) {
        string line;
        while (getline(f, line)) {
          spec_parser.Next(line);
        }
      },
      [&](ifstream& f) {
        json d = json();
        try {
          d = json::parse(f);
        } catch (...) {
        }
        content->data.Load(d);
      });
  return content;
}

void SaveWdbibData(const BibDataFile& file, const WdbibFileContent* content)
{
  const auto& spec = content->spec;
  const auto& data = content->data;

  if (spec.updated()) {
    file.SaveSpec([&](ofstream& f) {
      for (size_t i = 0; i < spec.Size(); ++i) {
        if (auto s = spec.Line(i); s) {
          auto rendered = (*s)->parsed->toString();
          if (rendered == (*s)->content) {
            f << (*s)->data;
          } else {
            f << rendered;
          }
          f << endl;
        }
      }
    });
  }
  if (data.updated()) {
    file.SaveData([&](ofstream& f) { f << data.Dump(); });
  }
}

}  // namespace file

string ParsedSpecVersionHeader::toString() 
{
  return absl::StrCat(version_);
}

extern const char* const kHeaderHintModifierDelimiter;
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

void ParsedSpecVersionHeader::PopulateSpecContent(
    SpecFileContent* content)
{
  content->set_version(&version_);
}

void ParsedSpecHintsHeader::PopulateSpecContent(
    SpecFileContent* content)
{
  content->set_hints(&hints_);
}

void ParsedSpecCitationBody::PopulateSpecContent(
    SpecFileContent* content)
{
  content->appendCitation(item_.qid);
}

}  // namespace wdbib
