#include "meta.h"

#include "../network/wikidata.h"
#include "../storage/data_file.h"

namespace wdbib {

void runSyncSubCommand()
{
  BibDataFile bib(kDefaultDataFilename);
  auto content = file::LoadWdbibData(bib);

  content.data.Update(GetWikidataItems(content.spec.QIDs()));
  file::SaveWdbibData(bib, content);
}

void SetupSyncSubCommand(CLI::App& app)
{
  auto cmd = app.add_subcommand("sync", "sync citation data with the library");

  cmd->callback([]() { runSyncSubCommand(); });
}

}  // namespace wdbib
