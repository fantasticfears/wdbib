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

def write_header(out):
  out.write(f"// GENERATED FILE. DON'T MODIFY.\n// See {__file__}\n")

def generate_props():
  with open(os.path.join(dir_path, 'props.json'), 'r') as f:
    props = json.load(f)
    with open(os.path.join(dir_path, '..', 'src', '_wikicite_property.gdh'), 'w') as out:
      write_header(out)
      output = [
          '{' + f'"{qid}", {TYPES[type]}' + '}' for qid, type in types.items()
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
