#include <catch.hpp>

#include <fstream>
#include <string>

#include "wikidata.h"

namespace wdbib {

std::string readFixtureFile(const std::string& path)
{
  std::ifstream fin(path);
  if (!fin.is_open()) {
    throw std::invalid_argument("Invalid path to load fixture from " + path);
  } else {
    fin.seekg(0, std::ios::end);
    size_t size = fin.tellg();
    std::string buffer(size, ' ');
    fin.seekg(0);
    fin.read(&buffer[0], size);
    return buffer;
  }
}

using Catch::Matchers::Equals;

TEST_CASE("parse a wikidata item", "[parsing]")
{
  auto buf = readFixtureFile("src/fixtures/Q163335.json");
  auto i = ParseWikidataJson("Q163335", buf);
  REQUIRE_THAT(i.id, Equals("Q163335"));
  REQUIRE_THAT(i.labels["en"][0], Equals("Principia Mathematica"));
  REQUIRE_THAT(
      i.descriptions["en"],
      Equals("three-volume work on the foundations of mathematics, written by "
             "Alfred North Whitehead and Bertrand Russell"));
  REQUIRE_THAT(i.claims["P31"].value, Equals("Q277759"));
}

}  // namespace wdbib
