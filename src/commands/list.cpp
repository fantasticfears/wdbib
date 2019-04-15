#include "list.h"

#include <utility>
#include <iostream>
#include <string_view>

#include <absl/strings/str_split.h>
#include "rang.hpp"

#include "../storage/data_file.h"

namespace wdbib {

void ProcessList()
{
  using namespace std;

  auto citation_file_content = file::ReadAll("citation");
  for (const auto& line : absl::StrSplit(citation_file_content, '\n')) {
    pair<string_view, string_view> p = absl::StrSplit(line, ':');
    if (p.first != "wc") {
      continue;
    }
    auto status = "local";
    constexpr auto kLocalStatus = "local";
    constexpr auto kCachedStatus = "cached";
    if (status == kLocalStatus) {
      cout << '[' << rang::fg::red << kLocalStatus << rang::fg::reset << "] ";
    } else {
      cout << '[' << rang::fg::magenta << kCachedStatus << rang::fg::reset << "] ";
    }
    cout << rang::fg::blue << p.second << rang::fg::reset << endl;
  }
}

} // namespace wdbib
