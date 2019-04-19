#pragma once
#ifndef WIKICITE_H_
#define WIKICITE_H_

#include "wikicite.h"

#include <string>

namespace wdbib {

using namespace std;
using json = nlohmann::json;

const unordered_map<string, WikiCiteItemType> gkWikiciteProps = {
#include "_wikicite_types.gdh"
};

const unordered_map<string, WikidataProperty> gkWikidataProp = {
#include "_wikicite_props.gdh"
};

namespace wd {

void from_json(const json& j, WikidataItem& p)
{
  j.at("id").get_to(p.id);
  auto labels = j.at("labels");
  for (auto& [key, value] : labels.items()) {
    p.labels[key].push_back(value.at("value").get<string>());
  }
  auto descriptions = j.at("descriptions");
  for (auto& [key, value] : descriptions.items()) {
    p.descriptions[key] = value.at("value").get<string>();
  }
  auto aliases = j.at("aliases");
  for (auto& [key, value] : aliases.items()) {
    p.labels[key].push_back(value.at("value").get<string>());
  }

  auto claims = j.at("claims");
  for (auto& [key, value] : claims.items()) {
    auto mainsnak = value.at(0).at("mainsnak");
    auto dt = mainsnak.find("datatype");
    if (dt == mainsnak.end()) {
      continue;
    }
    auto dv = mainsnak.at("datavalue").at("value");
    if (*dt == "wikibase-item") {
      auto obj = dv.at("id").get<string>();
      p.claims[key] = DataValue{DataValueType::WikibaseEntityId, obj};
    } else if (*dt == "string") {
      p.claims[key] = DataValue{DataValueType::String, dv.get<string>()};
    } else if (*dt == "monolingualtext") {
      p.claims[key] = DataValue{DataValueType::MonolingualText,
                                dv.at("text").get<string>()};
    } else if (*dt == "quantity") {
      p.claims[key] =
          DataValue{DataValueType::Quantity, dv.at("amount").get<string>()};
    } else if (*dt == "time") {
      p.claims[key] =
          DataValue{DataValueType::Time, dv.at("time").get<string>()};
    }
  }
}

}  // namespace wd

std::string JsonToBibTex(const nlohmann::json& j, const WdbibFileContent& content)
{
  WikidataItem item;
  wd::from_json(j, item);
  cite = WikidataToWikicite(item);

  string header;
  switch (cite.instance_of)
  {
  case WikiCiteItemType::kArticle:
    header += "@article{";
    break;
  
  case WikiCiteItemType::kMisc:
    [[:fallthrough]];
  default:
    header += "@misc{";
    break;
  }

  header += content.spec.Find(cite.qid)->toString();
  
  vector<string> body;
  body.push_back(fmt::format("title = {%s}", cite.title));
  return absl::StrCat(header, "{", absl::StrJoin(body, ','), "}\n");
}

WikiCiteItem WikidataToWikicite(const wd::WikidataItem& item)
{
  WikiCiteItem cite;

  cite.qid = item.id;

  /* we must understand the type because of its importance and we need to filter random entities. */
  try {
    auto instance_of_dv = item.claims[gkWikidataProp[WikidataProperty::kInstanceOf]];
  } catch (...) {
    throw WikidataParsingError("item doesn't have type information");
  }
  if (instance_of_dv.type == DataValueType::WikibaseEntityId) {
    try {
      cite.instance_of = gkWikiciteProps[instance_of_dv.value];
    } catch (...) {
      throw InvalidWikiciteItemError("not a valid wikicite item");
    }
  } else {
    throw WikidataParsingError("item has a different type than expected.");
  }

  cite.title = item.labels["en"].front();
  return cite;
}

}  // namespace wdbib

#endif  // WIKICITE_H_