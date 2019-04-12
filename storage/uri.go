package storage

import (
	"errors"
)

// URISchema describes how an item would be built
type URISchema struct {
	// Schema describes the uri schema
	Schema string
	// PathPattern descibes how the uri path would be layout.
	PathPattern string
}

// DefaultSchema is the default schema used
var DefaultSchema = "wd"

// DefaultPathPattern is the default schema
var DefaultPathPattern = "[identifier]:[author]"

// DefaultURISchema is the default uri schema building citation items if not otherwise defined.
var DefaultURISchema = &URISchema{
	Schema:      DefaultSchema,
	PathPattern: DefaultPathPattern,
}

// Citation stores the information identifiying a biliographical item
type Citation struct {
	// Identifier stores Q-identifier for lookup
	Identifier string
	// record stores the complete uri string
	record string
}

// CachedCitation stores the information identifiying a biliographical item
type CachedCitation struct {
	// Identifier stores Q-identifier for lookup
	Identifier string
}

func (s *URISchema) validateSchema() error {
	if s.Schema != DefaultSchema {
		return errors.New("no other schema supported")
	}
	return nil
}

// BuildCitationFromQId builds a uri for a bibliographic item based on Q-ID.
// qID represents Wikidata Q-ID, starts with `Q`
func (s *URISchema) BuildCitationFromQId(qID string) (Citation, error) {
	var cite Citation
	if err := s.validateSchema(); err != nil {
		return cite, err
	}

	return cite, nil
}
