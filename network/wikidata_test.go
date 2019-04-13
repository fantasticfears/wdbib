package network

import (
	"encoding/json"
	"testing"

	"github.com/fantasticfears/wdbib/testhelper"
)

func TestSnakParsing(t *testing.T) {
	buf := testhelper.LoadBytes(t, "Q163335_snak.json")
	var snak Snak
	json.Unmarshal(buf, &snak)
	testhelper.Assert(t, snak.Datatype == "wikibase-item", "can parse a snak")
	testhelper.Assert(t, snak.Datavalue.Value.(*WikibaseEntityIdDataValue).Id == "Q277759", "can parse a datavalue")
}

func TestClaimParsing(t *testing.T) {
	buf := testhelper.LoadBytes(t, "Q163335_claim.json")
	var claim Claim
	json.Unmarshal(buf, &claim)
	testhelper.Assert(t, claim.MainSnak.Datatype == "wikibase-item", "can parse a claim")
	testhelper.Assert(t, claim.Qualifiers["P2635"][0].Property == "P2635", "can parse a qualifier")
}
func TestParseWikidataResponse(t *testing.T) {
	buf := testhelper.LoadBytes(t, "Q163335.json")
	items, err := ParseWikidataResponse(buf)
	testhelper.Ok(t, err)
	item := items[0]

	claim := item.Claims["P31"][0]
	testhelper.Assert(t, claim.MainSnak.Datatype == "wikibase-item", "parses Type in mainsnak")
	testhelper.Assert(t, claim.MainSnak.Datavalue.Value.(*WikibaseEntityIdDataValue).Id == "Q277759", "can parse snak into wikidata type")
	q := claim.Qualifiers["P2635"][0]
	testhelper.Assert(t, q.Datatype == "quantity", "parses Type")
	testhelper.Assert(t, q.Datavalue.Value.(*QuantityDataValue).Amount == "+3", "parse the amount of data")
}
