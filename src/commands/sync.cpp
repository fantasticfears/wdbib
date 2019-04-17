#include "meta.h"

#include "../network/wikidata.h"
#include "../storage/data_file.h"

namespace wdbib {

void runSyncSubCommand()
{
  BibDataFile bib("citation");
  BibDataLockFile lock("citation.lock");

  auto c = bib.Parse();
  vector<string> qids;
  qids.reserve(c.items.size());
  for (const auto& item : c.items) {
    qids.push_back(item.qid);
  }
  lock.update(GetWikidataItems(qids));
  lock.Save();
}

void SetupSyncSubCommand(CLI::App& app)
{
  auto cmd = app.add_subcommand("sync", "sync citation data with the library");

  cmd->callback([]() { runSyncSubCommand(); });
}

}  // namespace wdbib
