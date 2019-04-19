# go here to get new table: https://github.com/citation-js/citation-js/tree/master/packages/plugin-wikidata/src

import json
import os
import os.path
import sys

dir_path = os.path.dirname(os.path.realpath(__file__))

TYPES = {
    "article": "WikiCiteItemType::kArticle",
    "article-newspaper": "WikiCiteItemType::kMisc",
    "article-journal": "WikiCiteItemType::kMisc",
    "article-magazine": "WikiCiteItemType::kMisc",
    "thesis": "WikiCiteItemType::kMisc",
    "dataset": "WikiCiteItemType::kMisc",
    "webpage": "WikiCiteItemType::kWebpage",
    "map": "WikiCiteItemType::kMisc",
    "book": "WikiCiteItemType::kBook",
    "chapter": "WikiCiteItemType::kMisc",
    "review": "WikiCiteItemType::kMisc",
    "review-book": "WikiCiteItemType::kMisc",
    "manuscript": "WikiCiteItemType::kMisc",
    "legal_case": "WikiCiteItemType::kMisc",
    "patent": "WikiCiteItemType::kMisc",
    "treaty": "WikiCiteItemType::kMisc",
    "bill": "WikiCiteItemType::kMisc",
    "report": "WikiCiteItemType::kMisc",
    "broadcast": "WikiCiteItemType::kMisc",
    "entry-dictionary": "WikiCiteItemType::kMisc",
    "entry-encyclopedia": "WikiCiteItemType::kMisc",
    "entry": "WikiCiteItemType::kMisc",
    "post": "WikiCiteItemType::kMisc",
    "post-weblog": "WikiCiteItemType::kMisc",
    "motion_picture": "WikiCiteItemType::kMisc",
    "song": "WikiCiteItemType::kMisc",
    "paper-conference": "WikiCiteItemType::kMisc",
    "interview": "WikiCiteItemType::kMisc",
    "speech": "WikiCiteItemType::kMisc",
    "musical_score": "WikiCiteItemType::kMisc",
}

PROPS = {
    "P31": "WikidataProperty::kInstanceOf",
    "P50": "WikidataProperty::kAuthor",
    "P57": "WikidataProperty::kDirector",
    "P86": "WikidataProperty::kComposer",
    "P98": "WikidataProperty::kEditor",
    "P110": "WikidataProperty::kIllustrator",
    "P123": "WikidataProperty::kPublisher",
    "P136": "WikidataProperty::kGenre",
    "P212": "WikidataProperty::kISBN",
    "P236": "WikidataProperty::kISSN",
    "P291": "WikidataProperty::kPublisherPlace",
    "P304": "WikidataProperty::kPage",
    "P348": "WikidataProperty::kVersion",
    "P356": "WikidataProperty::kDOI",
    "P393": "WikidataProperty::kEdition",
    "P433": "WikidataProperty::kIssue",
    "P478": "WikidataProperty::kVolume",
    "P577": "WikidataProperty::kIssued",
    "P655": "WikidataProperty::kTranslator",
    "P698": "WikidataProperty::kPMID",
    "P932": "WikidataProperty::kPMCID",
    "P953": "WikidataProperty::kURL",
    "P957": "WikidataProperty::kISBN",
    "P1104": "WikidataProperty::kNnumberOfPages",
    "P1433": "WikidataProperty::kContainerTitle",
    "P1476": "WikidataProperty::kTitle",
    "P2093": "WikidataProperty::kAuthor"
}

def write_header(out):
  out.write(f"// GENERATED FILE. DON'T MODIFY.\n// See {__file__}\n")

def generate_props():
  with open(os.path.join(dir_path, 'props.json'), 'r') as f:
    props = json.load(f)
    with open(os.path.join(dir_path, '..', 'src', '_wikicite_props.gdh'), 'w') as out:
      write_header(out)
      output = [
          '{' + f'{PROPS[pid]}, "{pid}"' + '}' for pid, _ in props["props"].items()
      ]
      out.write(',\n'.join(output))


def generate_types():
  with open(os.path.join(dir_path, 'types.json'), 'r') as f:
    types = json.load(f)
  with open(os.path.join(dir_path, '..', 'src', '_wikicite_types.gdh'), 'w') as out:
    write_header(out)
    output = [
        '{' + f'"{qid}", {TYPES[type]}' + '}' for qid, type in types.items()
    ]
    out.write(',\n'.join(output))


if __name__ == '__main__':
  if sys.argv[1] == "props":
    generate_props()
  if sys.argv[1] == "types":
    generate_types()
