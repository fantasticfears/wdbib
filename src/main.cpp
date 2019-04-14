#include <iostream>

#include <CLI/CLI.hpp>

#include "commands/version.h"

int main(int argc, char** argv)
{
  std::ios::sync_with_stdio(false);
  CLI::App app{"wdbib manages bibliographical library based on Wikidata."};

  auto addCmd = app.add_subcommand("add", "add a citation to the library");
  auto versionCmd = app.add_subcommand("version", "show version of the program.");
  versionCmd->callback(wdbib::VersionCallback);
  auto listCmd = app.add_subcommand("list", "lists all add a citation to the library.");
  auto searchCmd = app.add_subcommand("search", "Search bibliographical items on Wikidata.");

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError& e) {
    return app.exit(e);
  }
  return 0;
}
