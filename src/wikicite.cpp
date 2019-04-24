#include "wikicite.h"

#include <string>
#include <iostream>

#include <fmt/format.h>
#include <absl/strings/str_cat.h>
#include <absl/strings/str_join.h>

#include "errors.h"
#include "wdbib_data.h"

namespace wdbib {

using namespace std;
using json = nlohmann::json;

const unordered_map<string, WikiCiteItemType> gkWikiciteProps = {
#include "_wikicite_types.gdh"
};

const unordered_map<WikidataProperty, string> gkWikidataProp = {
#include "_wikicite_props.gdh"
};

namespace wd {

// See: https://www.mediawiki.org/wiki/Wikibase/DataModel/JSON
void from_json(const json& j, Snak& snak)
{
  auto dt = j.find("datatype");
  if (dt == j.end()) {
    return;
  }
  j.at("snaktype").get_to(snak.snaktype);
  auto dv = j.at("datavalue").at("value");
  if (*dt == "wikibase-item") {
    snak.dv.type = DataValueType::WikibaseEntityId;
    dv.at("id").get_to(snak.dv.value);
  } else if (*dt == "string") {
    snak.dv.type = DataValueType::String;
    dv.get_to(snak.dv.value);
  } else if (*dt == "monolingualtext") {
    snak.dv.type = DataValueType::MonolingualText;
    snak.dv.value = fmt::format("{}@{}", dv.at("text").get<string>(), dv.at("language").get<string>());
  } else if (*dt == "quantity") {
    snak.dv.type = DataValueType::Quantity;
    dv.at("amount").get_to(snak.dv.value);
  } else if (*dt == "time") {
    snak.dv.type = DataValueType::Time;
    dv.at("time").get_to(snak.dv.value);
  }
}

void from_json(const json& j, WikidataItem& p)
{
  j.at("id").get_to(p.id);
  try {
    for (auto& [key, value] : j.at("labels").items()) {
      p.labels[key].push_back(value.at("value").get<string>());
    }
  } catch (...) {
  }
  try {
    for (auto& [key, value] : j.at("descriptions").items()) {
      p.descriptions[key].push_back(value.at("value").get<string>());
    }
  } catch (...) {
  }
  try {
    for (auto& [key, value] : j.at("aliases").items()) {
      p.labels[key].push_back(value.at("value").get<string>());
    }
  } catch (...) {

  }

  try {
    for (auto& [key, values] : j.at("claims").items()) {
      try {
        if (!values.empty()){
          values.front().at("mainsnak").get_to(p.claims[key]);
        }
      } catch (...) {}
    }
  } catch (...) {
    
  }
}

}  // namespace wd

wd::WikidataItem TryParseWikidataJson(const nlohmann::json& j, const std::string qid)
{
  return j.at("entities").at(qid).get<wd::WikidataItem>();
}

std::string JsonToBibTex(const nlohmann::json& j, const WdbibFileContent* content)
{
  wd::WikidataItem item = j.get<wd::WikidataItem>();
  auto cite = WikidataToWikicite(item);
  if (!content->spec.Found(cite.qid)) {
    throw WikiciteItemNotFoundError(fmt::format("Item {} not fonud.", cite.qid));
  }

  string header;
  switch (cite.instance_of)
  {
  case WikiCiteItemType::kArticle:
    header += "@article{";
    break;
  
  case WikiCiteItemType::kMisc:
    [[fallthrough]];
  default:
    header += "@misc{";
    break;
  }

  header += content->spec.Find(cite.qid)->toString();
  
  vector<string> body;
  // TODO: refactor to output keys
  body.push_back(fmt::format("title = {}", cite.title));
  return absl::StrCat(header, ",\n", absl::StrJoin(body, ",\n"), "\n}\n");
}

WikiCiteItem WikidataToWikicite(const wd::WikidataItem& item)
{
  WikiCiteItem cite;

  cite.qid = item.id;

  /* we must understand the type because of its importance and we need to filter random entities. */
  try {
    auto& instance_of_dv = item.claims.at(gkWikidataProp.at(WikidataProperty::kInstanceOf)).dv;
    if (instance_of_dv.type == wd::DataValueType::WikibaseEntityId) {
      try {
        cite.instance_of = gkWikiciteProps.at(instance_of_dv.value);
      } catch (...) {
        throw InvalidWikiciteItemError("not a valid wikicite item");
      }
    } else {
      throw WikidataParsingError("item has a different type than expected.");
    }
  } catch (std::out_of_range) {
    throw WikidataParsingError("item doesn't have type information");
  }

  // cite.title = item.labels.at("en").front();
  return cite;
}

}  // namespace wdbib
