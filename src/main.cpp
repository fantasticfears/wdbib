#include <iostream>
#include <string>
#include <vector>

#include <CLI/CLI.hpp>
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include "commands/meta.h"
#include "commands/validators.h"

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

  CLI::App app{
      "wdbib - a program manages bibliographical library based on Wikidata."};
  app.require_subcommand(1);

  SetupAddSubCommand(app);
  SetupListSubCommand(app);
  SetupSyncSubCommand(app);
  SetupVersionSubCommand(app);
  SetupViewSubCommand(app);

  auto exit_code = 0;
  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError& e) {
    exit_code = app.exit(e);
  }
  return exit_code;
}
