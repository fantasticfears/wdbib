#include "list.h"

#include <utility>
#include <iostream>
#include <string_view>

#include <absl/strings/str_split.h>

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
    cout << p.second << endl;
  }
}

} // namespace wdbib
