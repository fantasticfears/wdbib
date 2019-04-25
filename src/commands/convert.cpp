#include "meta.h"

#include <fstream>

#include "spdlog/spdlog.h"

#include "../storage/data_file.h"
#include "../wikicite.h"
#include "../wdbib_data.h"

namespace wdbib {

using namespace std;

void SetupConvertSubCommand(CLI::App& app)
{
  auto cmd = app.add_subcommand("convert", "convert citation data to bibtex file.");
  ConvertSubCmdOpt opt;
  auto t = cmd->add_option("-t,--to-file", opt.output_path, "the path for file to be written");

  cmd->callback([&opt]() { RunConvertSubCommand(opt); });
}

void RunConvertSubCommand(const ConvertSubCmdOpt& opt)
{
  BibDataFile bib(kDefaultDataFilename, kDefaultCachedDataExtrension);
  auto content = file::LoadWdbibData(bib);

  auto qids = content->spec.QIDs();

  for (auto [k,v] : content->data.All()) {
    spdlog::get("stderr")->debug(k);
  }
  
  ofstream out(opt.output_path, ios::out | ios::trunc);
  for (const auto& qid : qids) {
    if (!content->data.Found(qid)) {
      continue;
    }
    json j = content->data.Find(qid);
    cout << JsonToBibTex(j, content.get());
    out << JsonToBibTex(j, content.get());
  }
}

}  // namespace wdbib
