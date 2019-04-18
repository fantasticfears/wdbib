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

void BibDataFile::Save(function<void(ofstream&)> spec,
                       function<void(ofstream&)> data)
{
  ofstream sf(spec_filename_);
  spec(sf);
  ofstream df(data_filename_);
  data(df);
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

  file.Save(
      [&](ofstream& f) {
        for (const auto& line : spec) {
          f << line;
        }
      },
      [&](ofstream& f) { f << data.Dump(); });
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

string ParsedSpecLineBody::toString() override { return {}; }

string ParsedSpecCitationBody::toString() override
{
  if (!item_.aux_info.empty()) {
    return absl::StrCat(item_.qid, kPathDelimiter, absl::StrJoin(item_.aux_info, kPathDelimiter));
  } else {
    return item_.qid;
  }
}

}  // namespace wdbib
