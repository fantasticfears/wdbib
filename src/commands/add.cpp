#include "meta.h"

#include <algorithm>
#include <iostream>
#include <memory>

#include <absl/strings/str_cat.h>
#include <fmt/format.h>

#include "../network/wikidata.h"
#include "../storage/data_file.h"
#include "output_helper.h"

namespace wdbib {

void SetupAddSubCommand(CLI::App& app)
{
  auto cmd = app.add_subcommand("add", "add a citation to the library");
  auto opt = std::make_shared<AddSubCmdOpt>();

  cmd->add_option("-q,--qid", opt->qids, "citation Q-identifier on Wikidata");
  cmd->callback([opt]() { RunAddSubCommand(*opt); });
}

void ProcessAddQID(const string& qid)
{
  auto item = GetWikiciteItem(qid);
  file::OverWrite("citation", fmt::format("wc:{0}:{1}", item.id,
                                          item.labels["en"].front()));
}

constexpr size_t kMaxThreadPoolSize = 8;

void RunAddSubCommand(const AddSubCmdOpt& opt)
{
  auto pool_size = std::max(opt.qids.size(), kMaxThreadPoolSize);
  spinners::MultiLineSpinner spinner(pool_size);

  for (const auto& qid : opt.qids) {
    spinner.register_append(
        {spinners::SpinnerStatus::kPending, absl::StrCat("Adding ", qid, "..."),
         absl::StrCat("Adding ", qid, " done"), [&]() { ProcessAddQID(qid); }});
  }
  spinner.LoopSpinner();
}

}  // namespace wdbib
