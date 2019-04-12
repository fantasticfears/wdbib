package storage

import (
	"errors"
)

// UriSchema describes how an item would be built
type UriSchema struct {
	// Schema describes the uri schema
	Schema string
	// PathPattern descibes how the uri path would be layout.
	PathPattern string
}

// DefaultSchema is the default schema used
var DefaultSchema = "wd"

// DefaultPathPattern is the default schema
var DefaultPathPattern = "[identifier]:[author]"

func (s *UriSchema) validateSchema() error {
	if s.Schema != DefaultSchema {
		return errors.New("no other schema supported")
	}
	return nil
}

// BuildURIFromQId builds a uri for a bibliographic item based on Q-ID.
// qID represents Wikidata Q-ID, starts with `Q`
func (s *UriSchema) BuildURIFromQId(qID string) error {
	if err := s.validateSchema(); err != nil {
		return err
	}

	return nil
}
