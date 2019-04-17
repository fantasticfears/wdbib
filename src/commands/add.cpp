#include "meta.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <unordered_set>

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

optional<Citation> requestCitationFromQID(const string& qid,
                                          const CitationHints& hints)
{
  auto item = GetWikiciteItem(qid);
  Citation cite{qid};
  return {cite};
}

constexpr size_t kMaxThreadPoolSize = 8;

void RunAddSubCommand(const AddSubCmdOpt& opt)
{
  auto pool_size = std::max(opt.qids.size(), kMaxThreadPoolSize);
  spinners::MultiLineSpinner spinner(pool_size);
  BibDataFile bib("citation");
  auto c = bib.Parse();

  unordered_set<string> stored_qids;
  for (const auto& i : c.items) {
    stored_qids.insert(i.qid);
  }

  vector<optional<Citation>> cites(opt.qids.size(), nullopt);
  int i = 0;
  for (const auto& qid : opt.qids) {
    if (stored_qids.find(qid) == stored_qids.end()) {
      spinner.register_append(
          {spinners::SpinnerStatus::kPending, absl::StrCat("Adding ", qid, "..."),
          absl::StrCat("Adding ", qid, " done"),
          [&, i]() { cites[i] = requestCitationFromQID(qid, c.path_spec); }});
    } else {
      spinner.register_append(
        {spinners::SpinnerStatus::kFinished, "",
         absl::StrCat("Found ", qid),
         []() {  }});
    }

    i++;
  }
  spinner.LoopSpinner();
  for (const auto& cite : cites) {
    if (cite) {
      c.items.push_back(*cite);

      string repr = absl::StrCat("wc:", cite->qid);
      absl::StrAppend(&repr, ":", absl::StrJoin(cite->aux_info, ":"));
      c.text.push_back(repr);
    }
  }
  bib.Save(c);
}

}  // namespace wdbib
