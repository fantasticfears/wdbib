#include "meta.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <unordered_set>
#include <utility>

#include <absl/strings/str_cat.h>
#include <absl/strings/str_join.h>
#include <fmt/format.h>

#include "../network/wikidata.h"
#include "../storage/data_file.h"
#include "output_helper.h"

namespace wdbib {

using namespace std;

void SetupAddSubCommand(CLI::App& app)
{
  auto cmd = app.add_subcommand("add", "add a citation to the library");
  auto opt = std::make_shared<AddSubCmdOpt>();

  cmd->add_option("-q,--qid", opt->qids, "citation Q-identifier on Wikidata");
  cmd->callback([opt]() { RunAddSubCommand(*opt); });
}

typedef pair<Citation, string> CitationResult;
optional<CitationResult> requestCitationFromQID(const string& qid,
                                          const CitationHints& hints)
{
  auto item = GetWikiciteItem(qid);
  Citation cite{item.first.id};
  return {make_pair(cite, item.second)};
}

constexpr size_t kMaxThreadPoolSize = 8;

void RunAddSubCommand(const AddSubCmdOpt& opt)
{
  auto pool_size = std::max(opt.qids.size(), kMaxThreadPoolSize);
  spinners::MultiLineSpinner spinner(pool_size);
  BibDataFile bib(kDefaultDataFilename);
  auto c = bib.Parse();

  unordered_set<string> stored_qids;
  for (const auto& i : c.items) {
    stored_qids.insert(i.qid);
  }

  vector<optional<CitationResult>> cites(opt.qids.size(), nullopt);
  int i = 0;
  for (const auto& qid : opt.qids) {
    if (stored_qids.find(qid) == stored_qids.end()) {
      spinner.register_append(
          {spinners::SpinnerStatus::kPending, absl::StrCat("Adding ", qid, "..."),
          absl::StrCat("Adding ", qid, " done"),
          [&, i]() {
            try {
              cites[i] = requestCitationFromQID(qid, c.path_spec);
            } catch (...) {

            }
               }});
    } else {
      spinner.register_append(
        {spinners::SpinnerStatus::kFinished, "",
         absl::StrCat("Found ", qid),
         []() {  }});
    }

    i++;
  }
  spinner.LoopSpinner();
  BibDataLockFile cached(kDefaultCachedDataFilename);
  for (const auto& cite : cites) {
    if (cite) {
      c.items.push_back(cite->first);

      string repr = absl::StrCat("wc:", cite->first.qid);
      absl::StrAppend(&repr, ":", absl::StrJoin(cite->first.aux_info, ":"));
      c.text.push_back(repr);
      cached.update(cite->second);
    }
  }
  bib.Save(c);
  cached.Save();
}

}  // namespace wdbib
