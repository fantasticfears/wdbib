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

  auto c = bib.Parse();
  for (const auto& item : c.items) {
    auto status = lock.Found(item.qid) ? "cached" : "local";
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
