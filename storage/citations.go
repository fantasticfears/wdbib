package storage

// CiteItem represents a citation item from Wikidata
type CiteItem struct {
	QID         string
	Title       string
	Description string
	Authors     []string
	Publisher   string
	Date        string
}
