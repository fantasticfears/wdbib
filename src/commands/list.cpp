#include "meta.h"

#include <iostream>
#include <string_view>
#include <utility>

#include <absl/strings/str_split.h>
#include "rang.hpp"

#include "../storage/data_file.h"

namespace wdbib {

void SetupListSubCommand(CLI::App& app)
{
  auto cmd =
      app.add_subcommand("list", "lists all add a citation to the library.");
  cmd->callback([]() { RunListSubCommand(); });
}

constexpr auto kLocalStatus = "local";
constexpr auto kCachedStatus = "cached";

void RunListSubCommand()
{
  using namespace std;

  BibDataFile bib(kDefaultDataFilename);
  BibDataLockFile lock(kDefaultCachedDataFilename);
  auto content = file::LoadWdbibData(bib);

  for (const auto& qid : content.spec.QIDs()) {
    auto status = content.data.Found(qid) ? "cached" : "local";
    if (status == kLocalStatus) {
      cout << '[' << rang::fg::red << kLocalStatus << rang::fg::reset << "] ";
    } else {
      cout << '[' << rang::fg::magenta << kCachedStatus << rang::fg::reset
           << "] ";
    }
    cout << rang::fg::blue << item.qid << rang::fg::reset << endl;
  }
}

}  // namespace wdbib
