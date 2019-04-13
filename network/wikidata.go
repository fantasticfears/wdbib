package network

import (
	"encoding/json"
)

type LangItem struct {
	Language string
	Value    string
}
type StringDataValue string
type QuantityDataValue struct {
	Amount     string
	UpperBound string
	LowerBound string
	Unit       string
}
type MonoLingualTextDataValue struct {
	Language string
	Text     string
}
type WikibaseEntityIdDataValue struct {
	EnitityType string `json:"entity-type"`
	Id          string
}
type TimeDataValue struct {
	Time          string
	Timezone      int64
	Before        int64
	After         int64
	Precision     int64
	CalendarModel string
}
type DataValue struct {
	Type  string `json:"type"`
	Value interface{}
}
type Snak struct {
	Snaktype  string
	Property  string
	Datatype  string
	Datavalue DataValue `json:",omitempty"`
}
type Claim struct {
	Type       string
	MainSnak   Snak
	Rank       string
	Qualifiers map[string][]Snak `json:"qualifiers,omitempty"`
}
type WikidataItem struct {
	Id           string
	Type         string
	Labels       map[string]LangItem
	Descriptions map[string]LangItem
	Claims       map[string][]Claim
}

func (s *Snak) decomposeSnak(datavalue json.RawMessage) error {
	switch s.Datatype {
	case "string":
		var str string
		if err := json.Unmarshal(datavalue, &str); err != nil {
			return err
		}
		s.Datavalue.Value = &str
	case "monolingualtext":
		var mlt MonoLingualTextDataValue
		if err := json.Unmarshal(datavalue, &mlt); err != nil {
			return err
		}
		s.Datavalue.Value = &mlt
	case "wikibase-item":
		var we WikibaseEntityIdDataValue
		if err := json.Unmarshal(datavalue, &we); err != nil {
			return err
		}
		s.Datavalue.Value = &we
	case "quantity":
		var q QuantityDataValue
		if err := json.Unmarshal(datavalue, &q); err != nil {
			return err
		}
		s.Datavalue.Value = &q
	case "time":
		var t TimeDataValue
		if err := json.Unmarshal(datavalue, &t); err != nil {
			return err
		}
		s.Datavalue.Value = &t
	}
	return nil
}

func (s *Snak) UnmarshalJSON(b []byte) error {
	var f map[string]json.RawMessage
	json.Unmarshal(b, &f)

	json.Unmarshal(f["snaktype"], &s.Snaktype)
	json.Unmarshal(f["datatype"], &s.Datatype)
	json.Unmarshal(f["property"], &s.Property)
	var dv json.RawMessage
	var dvItem = DataValue{
		Value: &dv,
	}
	json.Unmarshal(f["datavalue"], &dvItem)

	switch s.Datatype {
	case "string":
		var str string
		if err := json.Unmarshal(dv, &str); err != nil {
			return err
		}
		s.Datavalue.Value = &str
	case "monolingualtext":
		var mlt MonoLingualTextDataValue
		if err := json.Unmarshal(dv, &mlt); err != nil {
			return err
		}
		s.Datavalue.Value = &mlt
	case "wikibase-item":
		var we WikibaseEntityIdDataValue
		if err := json.Unmarshal(dv, &we); err != nil {
			return err
		}
		s.Datavalue.Value = &we
	case "quantity":
		var q QuantityDataValue
		if err := json.Unmarshal(dv, &q); err != nil {
			return err
		}
		s.Datavalue.Value = &q
	case "time":
		var t TimeDataValue
		if err := json.Unmarshal(dv, &t); err != nil {
			return err
		}
		s.Datavalue.Value = &t
	}
	return nil
}

type WikidataResponse struct {
	Entities map[string]WikidataItem
}

// ParseWikidataResponse parses a string object into a list of WikidataItem or report error
func ParseWikidataResponse(buf []byte) ([]WikidataItem, error) {
	var resp WikidataResponse
	var items = make([]WikidataItem, 0, 1)
	if err := json.Unmarshal(buf, &resp); err != nil {
		return items, err
	}

	for _, item := range resp.Entities {
		items = append(items, item)
	}
	return items, nil
}
