#include "add.h"

namespace wdbib {

void ProcessAddQID(const string& qID)
{
  auto item = GetWikiciteItem(qID);
  std::cout << item.id << std::endl;
}

}  // namespace wdbib
