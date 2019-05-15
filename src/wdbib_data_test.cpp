#include "spec_stateful_parser.h"

#include <catch.hpp>

#include <sstream>

#include "errors.h"
#include "wdbib_data.h"

namespace wdbib {

using namespace Catch;

TEST_CASE("SpecFileContent can dump the data")
{
  SpecFileContent content;
  SpecStatefulParser p(&content);
  p.Next("Q13");

  REQUIRE_THAT(content.Dump(), Equals("Q13\n"));
}

TEST_CASE("round trip parsing citation")
{
  SpecFileContent content;
  SpecStatefulParser p(&content);

  BibDataFile file("src/fixtures/citation-round-trip-1", "lock");
  stringstream load;
  file.LoadSpec([&](ifstream& f) {
    string line;
    while (getline(f, line)) {
      load << line << "\n";
      p.Next(line);
    }
  });
  REQUIRE(content.Found("Q1633352"));
  string file_content = load.str();
  REQUIRE_THAT(content.Dump(), Equals(file_content));
}

TEST_CASE("manages qids")
{
  SECTION("can add/remove qids")
  {
    SpecFileContent content;
    SpecStatefulParser p(&content);

    BibDataFile file("src/fixtures/citation.example", "lock");
    stringstream load;
    file.LoadSpec([&](ifstream& f) {
      string line;
      while (getline(f, line)) {
        load << line;
        p.Next(line);
      }
    });
    REQUIRE_THAT(content.QIDs(), VectorContains(string("Q163335")));
  }
}

}  // namespace wdbib
