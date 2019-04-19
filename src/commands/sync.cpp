#include "meta.h"

#include "../network/wikidata.h"
#include "../storage/data_file.h"
#include "../wdbib_data.h"

namespace wdbib {

void runSyncSubCommand()
{
  BibDataFile bib(kDefaultDataFilename, kDefaultCachedDataExtrension);
  auto content = file::LoadWdbibData(bib);

  content->data.Update(GetWikidataItems(content->spec.QIDs()));
  file::SaveWdbibData(bib, content.get());
}

void SetupSyncSubCommand(CLI::App& app)
{
  auto cmd = app.add_subcommand("sync", "sync citation data with the library");

  cmd->callback([]() { runSyncSubCommand(); });
}

}  // namespace wdbib
