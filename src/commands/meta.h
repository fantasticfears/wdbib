#pragma once

#include <vector>
#include <string>

#include <CLI/CLI.hpp>

namespace wdbib {

struct AddSubCmdOpt
{
  std::vector<std::string> qids;
};
void SetupAddSubCommand(CLI::App& app);
void RunAddSubCommand(const AddSubCmdOpt& opt);

void SetupListSubCommand(CLI::App& app);
void RunListSubCommand();

void SetupVersionSubCommand(CLI::App& app);

void SetupSearchSubCommand(CLI::App& app);

struct ViewSubCmdOpt
{
  bool wikidata = false;
  bool scholia = false;
  std::string qid;
};
void SetupViewSubCommand(CLI::App& app);
void RunViewSubCommand(const ViewSubCmdOpt& opt);

}  // namespace wdbib