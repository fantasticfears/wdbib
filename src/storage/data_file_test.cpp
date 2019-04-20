#include <catch.hpp>

#include <optional>
#include <string>

#include "../errors.h"
#include "data_file.h"

namespace wdbib {

using namespace Catch;
using namespace std;

TEST_CASE("parse the spec", "[parsing]")
{
  REQUIRE_THROWS_AS(TryParseSpecWithDefault(" x", content::kDefaultSpec, 1),
                    ParsingError);
  REQUIRE_THROWS_AS(TryParseSpecWithDefault(" ", content::kDefaultSpec, 1),
                    ParsingError);

  REQUIRE_THROWS_AS(TryParseSpecWithDefault("2", content::kDefaultSpec, 1),
                    InvalidHeaderError);
  REQUIRE_THROWS_AS(TryParseSpecWithDefault(" 2", content::kDefaultSpec, 1),
                    InvalidHeaderError);

  REQUIRE(TryParseSpecWithDefault(" 1", content::kDefaultSpec, 1) == "1");
  REQUIRE(TryParseSpecWithDefault("1", content::kDefaultSpec, 1) == "1");
  REQUIRE(TryParseSpecWithDefault(" 1 ", content::kDefaultSpec, 1) == "1");
}

TEST_CASE("parse the hints", "[parsing]")
{
  REQUIRE_THROWS_AS(TryParseHintsWithDefault(" [k", content::kDefaultHints, 1),
                    ParsingError);

  REQUIRE_THROWS_AS(TryParseHintsWithDefault(" s]", content::kDefaultHints, 1),
                    ParsingError);

  REQUIRE_THAT(
      TryParseHintsWithDefault(" [title] ", content::kDefaultHints, 1)[0],
      Equals("title"));
}

TEST_CASE("parse the citation", "[parsing]")
{
  REQUIRE_THROWS_AS(ParseCitation({"title"}, "wc:", 1), ParsingError);
  REQUIRE_THROWS_AS(ParseCitation({"title"}, "wc1:Q22", 1), ParsingError);

  REQUIRE(ParseCitation({}, "", 1) == nullopt);
  REQUIRE_THAT(ParseCitation({}, "wc:Q222", 1)->qid, Equals("Q222"));
  auto c1 = *ParseCitation({"title"}, "wc:Q222:ta", 1);
  REQUIRE_THAT(c1.qid, Equals("Q222"));
  REQUIRE_THAT(c1.aux_info[0], Equals("ta"));
}

TEST_CASE("parse the bib file", "[parsing]")
{
  BibDataFile bib("src/fixtures/citation.example");
  auto c = bib.Parse();

  REQUIRE_THAT(c.spec, Equals("1"));
  REQUIRE_THAT(c.path_spec[0], Equals("title"));

  REQUIRE_THAT(c.items[0].qid, Equals("Q163335"));
}

}  // namespace wdbib
