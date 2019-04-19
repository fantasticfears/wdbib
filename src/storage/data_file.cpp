#include "data_file.h"

#include <fstream>
#include <iostream>
#include <utility>

#include "../wdbib_data.h"
#include "spec_stateful_parser.h"

namespace wdbib {

using namespace std;

BibDataFile::BibDataFile(const string& filename, const string& lock_ext)
    : spec_filename_(filename),
      data_filename_(absl::StrCat(filename, '.' lock_ext))
{}

void BibDataFile::Load(function<void(ifstream&)> spec,
                       function<void(ifstream&)> data)
{
  ifstream sf(spec_filename_);
  spec(sf);
  ifstream df(data_filename_);
  data(df);
}

void BibDataFile::SaveAll(function<void(ofstream&)> spec,
                          function<void(ofstream&)> data)
{
  SaveSpec(spec);
  SaveData(data);
}

void BibDataFile::SaveSpec(function<void(std::ofstream&)> spec)
{
  ofstream sf(spec_filename_, std::ios::out | std::ios::trunc);
  spec(sf);
}

void BibDataFile::SaveData(function<void(std::ofstream&)> data)
{
  ofstream df(data_filename_, std::ios::out | std::ios::trunc);
  data(df);
}

SpecLine MakeSpecLine(const Citation& item)
{
  return {"", "", make_unique<ParsedSpecCitationBody>(item)};
}

namespace file {

WdbibFileContent LoadWdbibData(const BibDataFile& file)
{
  WdbibFileContent content;

  SpecStatefulParser spec_parser(&content.spec);
  file.Load(
      [&](ifstream& f) {
        string line;
        while (getline(line, f)) {
          spec_parser.Next(line);
        }
      },
      [&](ifstream& f) {
        json d;
        try {
          d = json::parse(f);
        } catch (...) {
          d = json();
        }
        content.data.Load(d);
      });
  return std::move(content);
}

void SaveWdbibData(const BibDataFile& file, const WdbibFileContent& content)
{
  const auto& spec = content.spec;
  const auto& data = content.data;

  if (spec.updated()) {
    file.SaveSpec([&](ofstream& f) {
      for (auto i = 0; i < spec.Size(); ++i) {
        if (auto s = spec.Line(i); s) {
          auto rendered = s->parsed->toString();
          if (rendered == s->content) {
            f << s->data;
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

string ParsedSpecVersionHeader::toString() override
{
  return absl::StrCat(version_);
}

struct HintsFormatter
{
  void operator()(std::string* out, const Hints& hint) const
  {
    switch (hint.type) {
    case HintType::kArticle:
      StrAppend(out, "article");
      break;
    default:
      break;
    }
    switch (hint.modifier) {
    case HintModifier::kFirstWord:
      StrAppend(out, kHeaderHintModifierDelimiter, "first word");
      break;

    default:
      break;
    }
  }
};

string ParsedSpecHintsHeader::toString() override
{
  return absl::StrJoin(hints_, kHeaderHintsDelimiter, HintsFormatter());
}

string ParsedSpecCitationBody::toString() override
{
  if (!item_.aux_info.empty()) {
    return absl::StrCat(item_.qid, kPathDelimiter,
                        absl::StrJoin(item_.aux_info, kPathDelimiter));
  } else {
    return item_.qid;
  }
}

void ParsedSpecVersionHeader::PopulateSpecContent(
    SpecFileContent* content) override
{
  content->set_version(&version_);
}

void ParsedSpecHintsHeader::PopulateSpecContent(
    SpecFileContent* content) override
{
  content->set_hints(&hints_);
}

void ParsedSpecCitationBody::PopulateSpecContent(
    SpecFileContent* content) override
{
  content.appendCitation(item_.qid);
}

}  // namespace wdbib
