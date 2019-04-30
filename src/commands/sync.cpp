#include "meta.h"

#include <iostream>
#include <optional>

#include <absl/strings/str_cat.h>
#include "spdlog/spdlog.h"

#include "../network/wikidata.h"
#include "../storage/data_file.h"
#include "../wdbib_data.h"
#include "output_helper.h"

namespace wdbib {
typedef std::pair<Citation, json> CitationResult;
void runSyncSubCommand()
{
  BibDataFile bib(kDefaultDataFilename, kDefaultCachedDataExtrension);
  auto content = file::LoadWdbibData(bib);

  vector<optional<CitationResult>> cites(content->spec.QIDs().size(), nullopt);
  auto pool_size = std::max(content->spec.QIDs().size(), kMaxThreadPoolSize);
  spinners::MultiLineSpinner spinner(pool_size);
  int i = 0;
  for (const auto& qid : content->spec.QIDs()) {
    spinner.register_append({spinners::SpinnerStatus::kPending,
                             absl::StrCat("Syncing ", qid, "..."),
                             absl::StrCat("Syncing ", qid, " done"),
                             [&, i](spinners::SpinnerObject* obj) {
                               try {
                                 auto item = GetWikidataItem(qid);
                                 Citation cite{item.first.id, {}};
                                 cites[i] = {make_pair(cite, item.second)};

                               } catch (...) {
                               }
                             }});
    i++;
  }

  spinner.LoopSpinner();

  for (const auto& cite : cites) {
    if (cite) {
      content->data.Update(cite->second);
    }
  }
  file::SaveWdbibData(bib, content.get());
}

void SetupSyncSubCommand(CLI::App& app)
{
  auto cmd = app.add_subcommand("sync", "sync citation data with the library");

  cmd->callback([]() { runSyncSubCommand(); });
}

}  // namespace wdbib
