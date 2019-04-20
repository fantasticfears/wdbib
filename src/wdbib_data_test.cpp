#include "spec_stateful_parser.h"

#include <catch.hpp>

#include <sstream>

#include "errors.h"
#include "wdbib_data.h"

namespace wdbib {

using namespace Catch;

TEST_CASE("round trip parsing citation")
{
  SECTION("can jump to body state if met non-header line in the beginning")
  {
    SpecFileContent content;
    SpecStatefulParser p(&content);

    BibDataFile file("src/fixtures/citation-round-trip-1", "lock");
    stringstream load;
    file.LoadSpec([&](ifstream& f) {
      string line;
      while (getline(f, line)) {
        load << line;
        p.Next(line);
      }
    });
    REQUIRE_THAT(content.Dump(), Equals(load.str()));
  }
}

}  // namespace wdbib
