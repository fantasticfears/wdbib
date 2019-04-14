#define CATCH_CONFIG_MAIN

#include <catch.hpp>

#include "system.h"

namespace wdbib {

using Catch::Matchers::StartsWith;

TEST_CASE("get system information", "[meta]")
{
  REQUIRE_THAT(GetUserAgent(), StartsWith("wdbib"));
}

}  // namespace wdbib
