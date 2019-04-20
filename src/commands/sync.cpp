#include "meta.h"

#include "../network/wikidata.h"
#include "../storage/data_file.h"
#include "../wdbib_data.h"

#include <iostream>

namespace wdbib {

void runSyncSubCommand()
{
  BibDataFile bib(kDefaultDataFilename, kDefaultCachedDataExtrension);
  auto content = file::LoadWdbibData(bib);

  cout << content->spec.QIDs()[0] ;
  json parsed = json::parse(GetWikidataItems(content->spec.QIDs()));
  for (const auto& [k,v] : parsed.at("entities").items()) {
    content->data.Update(v);
  }
  file::SaveWdbibData(bib, content.get());
}

void SetupSyncSubCommand(CLI::App& app)
{
  auto cmd = app.add_subcommand("sync", "sync citation data with the library");

  cmd->callback([]() { runSyncSubCommand(); });
}

}  // namespace wdbib
