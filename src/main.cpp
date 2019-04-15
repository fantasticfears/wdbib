#include <iostream>
#include <string>
#include <vector>

#include <CLI/CLI.hpp>
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include "commands/add.h"
#include "commands/list.h"
#include "commands/version.h"

using std::string;
using std::vector;
using namespace wdbib;

int main(int argc, char** argv)
{
  std::ios::sync_with_stdio(false);
  auto network = spdlog::stderr_color_mt("network");
  // network->set_level(spdlog::level::debug);
  auto err_logger = spdlog::stderr_color_mt("stderr");
  err_logger->set_level(spdlog::level::debug);

  CLI::App app{"wdbib - a program manages bibliographical library based on Wikidata."};

  auto add_cmd = app.add_subcommand("add", "add a citation to the library");
  vector<string> add_qIDs;
  add_cmd->add_option("-q,--qid", add_qIDs,
                      "citation Q-identifier on Wikidata");
  add_cmd->callback([&]() {
    ProcessAddQIDs(add_qIDs);
  });

  auto version_cmd =
      app.add_subcommand("version", "show version of the program.");
  version_cmd->callback(wdbib::VersionCallback);

  auto list_cmd =
      app.add_subcommand("list", "lists all add a citation to the library.");
  list_cmd->callback([&]() {
    ProcessList();
  });

  auto search_cmd =
      app.add_subcommand("search", "Search bibliographical items on Wikidata.");

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError& e) {
    return app.exit(e);
  }
  return 0;
}
