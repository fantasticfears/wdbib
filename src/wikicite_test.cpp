#include <catch.hpp>

#include "wikicite.h"

#include <fstream>
#include <string>

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

TEST_CASE("parse a wikidata item's id", "[parsing]")
{
  auto buf = json::parse(readFixtureFile("src/fixtures/Q163335.json.id"));
  wd::WikidataItem i;
  wd::from_json(buf, i);
  REQUIRE_THAT(i.id, Equals("Q163335"));
}

TEST_CASE("parse a wikidata's snak", "[parsing]")
{
  auto buf = json::parse(readFixtureFile("src/fixtures/Q163335.json.snak"));
  wd::Snak snak;
  wd::from_json(buf, snak);
  REQUIRE(snak.dv.type == wd::DataValueType::WikibaseEntityId);
  REQUIRE_THAT(snak.snaktype, Equals("value"));
  REQUIRE_THAT(snak.dv.value, Equals("Q277759"));
}

TEST_CASE("parse a wikidata item's literals", "[parsing]")
{
  auto buf = json::parse(readFixtureFile("src/fixtures/Q163335.json"));
  wd::WikidataItem item;
  wd::from_json(buf, item);
  REQUIRE_THAT(item.labels["en"][0], Equals("Principia Mathematica"));
  REQUIRE_THAT(item.descriptions["en"][0], Equals("three-volume work on the foundations of mathematics, written by Alfred North Whitehead and Bertrand Russell"));
}

TEST_CASE("parse a wikidata item's snak", "[parsing]")
{
  auto buf = json::parse(readFixtureFile("src/fixtures/Q163335.json"));
  wd::WikidataItem item;
  wd::from_json(buf, item);
  REQUIRE_THAT(item.claims["P31"].snaktype, Equals("value"));
  REQUIRE(item.claims["P31"].dv.type == wd::DataValueType::WikibaseEntityId);
  REQUIRE_THAT(item.claims["P31"].dv.value, Equals("Q277759"));
}

TEST_CASE("parse a wikidata item from API", "[parsing]")
{
  auto buf = json::parse(readFixtureFile("src/fixtures/Q163335.json.downloaded"));
  REQUIRE_NOTHROW(TryParseWikidataJson(buf, "Q163335"));
  REQUIRE_THROWS(TryParseWikidataJson(buf, "Q1"));
}

TEST_CASE("loads Wikidata property table", "[!hide][data]")
{
}

TEST_CASE("transform a wikidata item to wikicite item", "[transformation]")
{
  SECTION("when an entity is not a wikicite item") {
    auto buf = json::parse(readFixtureFile("src/fixtures/Q163335.json.downloaded"));
    auto item = TryParseWikidataJson(buf, "Q163335");
    REQUIRE_THROWS(WikidataToWikicite(item));
  }

  SECTION("when an entity is a wikicite item") {
    auto buf = json::parse(readFixtureFile("src/fixtures/Q21090025.json.downloaded"));
    auto item = TryParseWikidataJson(buf, "Q21090025");
    auto cite = WikidataToWikicite(item);
    REQUIRE_THAT(cite.qid, Equals("Q21090025"));
    REQUIRE(cite.instance_of == WikiCiteItemType::kArticle);
  }
  
}

}  // namespace wdbib
