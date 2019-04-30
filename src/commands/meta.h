#pragma once
#ifndef COMMANDS_META_H_
#define COMMANDS_META_H_

#include <vector>
#include <string>

#include <CLI/CLI.hpp>

namespace wdbib {

extern const size_t kMaxThreadPoolSize;

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

void SetupSyncSubCommand(CLI::App& app);

struct ConvertSubCmdOpt
{
  std::string output_path;
};
void SetupConvertSubCommand(CLI::App& app);
void RunConvertSubCommand(const ConvertSubCmdOpt& opt);

}  // namespace wdbib

#endif  // COMMANDS_META_H_
