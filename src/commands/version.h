#include <iostream>

#include "../system.h"

namespace wdbib {

void VersionCallback()
{
  auto ua = GetUserAgent();
  std::cout << ua << std::endl;
}

} // namespace wdbib
