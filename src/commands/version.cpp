#include "meta.h"

#include <iostream>

#include "../system.h"

namespace wdbib {

void SetupVersionSubCommand(CLI::App& app)
{
  auto cmd = app.add_subcommand("version", "show version of the program.");
  cmd->callback([]() {
    auto ua = GetUserAgent();
    std::cout << ua << std::endl;
  });
}

} // namespace wdbib
