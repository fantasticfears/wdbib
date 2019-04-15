#include "meta.h"

namespace wdbib {

void SetupSearchSubCommand(CLI::App& app)
{
  auto cmd =
      app.add_subcommand("search", "Search bibliographical items on Wikidata.");
}

} // namespace wdbib
