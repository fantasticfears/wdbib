#pragma once
#ifndef NETWORK_WIKIDATA_H_
#define NETWORK_WIKIDATA_H_

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>

namespace wdbib {

using nlohmann::json;
using std::string;
using std::unordered_map;
using std::vector;

namespace wd {

enum class DataValueType
{
  WikibaseEntityId,
  String,
  MonolingualText,
  Quantity,
  Time
};
struct DataValue
{
  DataValueType type;
  string value;
};

struct WikidataItem
{
  string id;
  unordered_map<string, vector<string>> labels;
  unordered_map<string, string> descriptions;
  unordered_map<string, DataValue> claims;
};

// TODO
// void to_json(json& j, const WikidataItem& p) {
// }

void from_json(const json& j, WikidataItem& p);

}  // namespace wd

wd::WikidataItem ParseWikiciteJson(const string& id, const string& json_str);

/*
 * Performs a network request to Wikidata and downloads file.
 * Throws exceptions.
 */
std::pair<wd::WikidataItem, string> GetWikiciteItem(const string& qID);

string GetWikidataItems(const string& query_ids);
string GetWikidataItems(const vector<string>& q_ids);

}  // namespace wdbib

#endif  // NETWORK_WIKIDATA_H_
