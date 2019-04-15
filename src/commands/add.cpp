#include "add.h"

#include <fmt/format.h>

#include "../storage/data_file.h"

namespace wdbib {

void ProcessAddQID(const string& qID)
{
  auto item = GetWikiciteItem(qID);
  file::OverWrite("citation", fmt::format("wc:{0}:{1}", item.id, item.labels["en"].front()));
}

}  // namespace wdbib
