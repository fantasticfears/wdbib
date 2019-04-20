#include "spec_stateful_parser.h"

#include <catch.hpp>

#include "../wdbib_data.h"
#include "../errors.h"

namespace wdbib {

TEST_CASE("manages the parsing state", "[state]")
{
  SECTION("can jump to body state if met non-header line in the beginning")
  {
    SpecFileContent content;
    SpecStatefulParser p(&content);
    REQUIRE(p.status() == ParserStatus::kStart);

    p.Next("");
    REQUIRE(p.status() == ParserStatus::kBody);
  }

  SECTION("can jump to header/body state")
  {
    SpecFileContent content;
    SpecStatefulParser p(&content);

    p.Next("#");
    REQUIRE(p.status() == ParserStatus::kHeader);
    p.Next("#");
    REQUIRE(p.status() == ParserStatus::kHeader);
    p.Next("");
    REQUIRE(p.status() == ParserStatus::kBody);
  }
}

TEST_CASE("parses the header", "[parsing][state]")
{
  SECTION("can throw if malformed version")
  {
    SpecFileContent content;
    SpecStatefulParser p(&content);

    REQUIRE_THROWS_AS(p.Next("#   version"), ParsingError);
  }

  SECTION("can throw if malformed version")
  {
    SpecFileContent content;
    SpecStatefulParser p(&content);

    REQUIRE_THROWS_AS(p.Next("#   version"), ParsingError);
  }

  SECTION("can throw if version is not supported")
  {
    SpecFileContent content;
    SpecStatefulParser p(&content);

    REQUIRE_THROWS_AS(p.Next("#   version: 2"), InvalidHeaderError);
  }

  SECTION("can parse a version")
  {
    SpecFileContent content;
    SpecStatefulParser p(&content);

    p.Next("#  version: 1");
    REQUIRE(*content.version() == 1);
  }
}

}  // namespace wdbib
