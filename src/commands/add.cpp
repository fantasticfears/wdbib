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
#include "../wdbib_data.h"
#include "output_helper.h"

namespace wdbib {

using namespace std;
const size_t kMaxThreadPoolSize = 8;

void SetupAddSubCommand(CLI::App& app)
{
  auto cmd = app.add_subcommand("add", "add a citation to the library");
  auto opt = std::make_shared<AddSubCmdOpt>();

  cmd->add_option("-q,--qid", opt->qids, "citation Q-identifier on Wikidata");
  cmd->callback([opt]() { RunAddSubCommand(*opt); });
}

typedef std::pair<Citation, json> CitationResult;
optional<CitationResult> requestCitationFromQID(const string& qid)
{
  auto item = GetWikidataItem(qid);
  Citation cite{item.first.id, {}};

  return {make_pair(cite, item.second)};
}

void verifyAddOpt(const AddSubCmdOpt& opt)
{
  if (opt.qids.empty()) {
    throw CLI::RequiresError(
        "Nothing specified, nothing added.\n\n"
        "A citation item is needed. Try:\n"
        "  wdbib add -q Q24347512\n"
        "  wdbib add -q Q36317269 Q24347512\n",
        1);
  }
}

void RunAddSubCommand(const AddSubCmdOpt& opt)
{
  verifyAddOpt(opt);

  BibDataFile bib(kDefaultDataFilename, kDefaultCachedDataExtrension);
  auto content = file::LoadWdbibData(bib);

  vector<optional<CitationResult>> cites(opt.qids.size(), nullopt);

  if (opt.qids.size() > 1) {
    auto pool_size = std::max(opt.qids.size(), kMaxThreadPoolSize);
    spinners::MultiLineSpinner spinner(pool_size);
    int i = 0;
    for (const auto& qid : opt.qids) {
      if (!content->spec.Found(qid)) {
        spinner.register_append({spinners::SpinnerStatus::kPending,
                                absl::StrCat("Adding ", qid, "..."),
                                absl::StrCat("Adding ", qid, " done"), [&, i](spinners::SpinnerObject*) {
                                  try {
                                    cites[i] = requestCitationFromQID(qid);
                                  } catch (...) {
                                  }
                                }});
      } else {
        spinner.register_append({spinners::SpinnerStatus::kFinished, "",
                                absl::StrCat("Found ", qid), [](spinners::SpinnerObject*) {}});
      }
      i++;
    }
    spinner.LoopSpinner();
  } else {
    const auto& qid = opt.qids.front();
    cout << "Adding " << qid << "..." << "\r";
    cites[0] = requestCitationFromQID(qid);
    cout << "Adding " << qid << " done" << endl;
  }

  for (const auto& cite : cites) {
    if (cite) {
      content->spec.Append(GetSpecLineFromCitation(cite->first));
      content->data.Update(cite->second);
    }
  }
  file::SaveWdbibData(bib, content.get());
}

}  // namespace wdbib
