wdbib: Ciatation management for Biber with Wikidata
====

`wdbib` is a command line programs helping your to manage citations in files based on open data from Wikidata.

It can:

- Store citation keys
- Be in the pipeline for Biber consumption
- Search citation data on Wikidata

## Files

The tool adds two files in the local directory where `wdbib` was called. 

- `citation`: a file stores Q-identifier with customization information. Each line defines a key for citation.
- `citation.lock`: the actual data fetched and stored from Wikidata. The data you can rely on and put on source control. It doesn't change overtime unless you want to.

## Commands

### Add files: `wdbib add`

To add files from Wikidata, use: `wdbib add -q Q29547544 Q36317269`.

### Sync citation data: `wdbib sync`

### List citations: `wdbib list`

### View a item on browser: `wdbib view`