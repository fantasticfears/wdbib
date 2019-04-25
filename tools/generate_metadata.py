# go here to get new table: https://github.com/citation-js/citation-js/tree/master/packages/plugin-wikidata/src

import json
import os
import os.path
import sys

dir_path = os.path.dirname(os.path.realpath(__file__))

TYPES = {
    "article": "kArticle",
    "article-newspaper": "kMisc",
    "article-journal": "kArticle",
    "article-magazine": "kMisc",
    "thesis": "kMisc",
    "dataset": "kMisc",
    "webpage": "kWebpage",
    "map": "kMisc",
    "book": "kBook",
    "chapter": "kMisc",
    "review": "kMisc",
    "review-book": "kMisc",
    "manuscript": "kMisc",
    "legal_case": "kMisc",
    "patent": "kMisc",
    "treaty": "kMisc",
    "bill": "kMisc",
    "report": "kMisc",
    "broadcast": "kMisc",
    "entry-dictionary": "kMisc",
    "entry-encyclopedia": "kMisc",
    "entry": "kMisc",
    "post": "kMisc",
    "post-weblog": "kMisc",
    "motion_picture": "kMisc",
    "song": "kMisc",
    "paper-conference": "kMisc",
    "interview": "kMisc",
    "speech": "kMisc",
    "musical_score": "kMisc",
}

PROPS = {
    "P31": "kInstanceOf",
    "P50": "kAuthor",
    "P57": "kDirector",
    "P86": "kComposer",
    "P98": "kEditor",
    "P110": "kIllustrator",
    "P123": "kPublisher",
    "P136": "kGenre",
    "P212": "kISBN",
    "P236": "kISSN",
    "P291": "kPublisherPlace",
    "P304": "kPage",
    "P348": "kVersion",
    "P356": "kDOI",
    "P393": "kEdition",
    "P433": "kIssue",
    "P478": "kVolume",
    "P577": "kIssued",
    "P655": "kTranslator",
    "P698": "kPMID",
    "P932": "kPMCID",
    "P953": "kURL",
    "P957": "kISBN",
    "P1104": "kNnumberOfPages",
    "P1433": "kContainerTitle",
    "P1476": "kTitle",
    "P2093": "kAuthor"
}

def write_header(out):
  out.write(f"// GENERATED FILE. DON'T MODIFY.\n// See {__file__}\n")

def generate_props():
  with open(os.path.join(dir_path, 'props.json'), 'r') as f:
    props = json.load(f)
    
  prefix = "WikidataProperty::"
  with open(os.path.join(dir_path, '..', 'src', '_wikicite_props.gdh'), 'w') as out:
    write_header(out)
    output = [
        '{' + f'{prefix}{PROPS[pid]}, "{pid}"' + '}' for pid, _ in props["props"].items()
    ]
    out.write(',\n'.join(output))
  
  with open(os.path.join(dir_path, '..', 'src', '_wikicite_props_enum.gdh'), 'w') as out:
    write_header(out)
    output = list(set(PROPS.values()))
    out.write(',\n'.join(output))


def generate_types():
  with open(os.path.join(dir_path, 'types.json'), 'r') as f:
    types = json.load(f)

  prefix = "WikiCiteItemType::"
  with open(os.path.join(dir_path, '..', 'src', '_wikicite_types.gdh'), 'w') as out:
    write_header(out)
    output = [
        '{' + f'"{qid}", {prefix}{TYPES[type]}' + '}' for qid, type in types.items()
    ]
    out.write(',\n'.join(output))
  with open(os.path.join(dir_path, '..', 'src', '_wikicite_types_enum.gdh'), 'w') as out:
    write_header(out)
    output = list(set(TYPES.values()))
    out.write(",\n".join(output))


if __name__ == '__main__':
  if len(sys.argv) > 1:
    if sys.argv[1] == "props":
      generate_props()
    elif sys.argv[1] == "types":
      generate_types()
  else:
    generate_props()
    generate_types()