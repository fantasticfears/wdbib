#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <utility>

#include <nlohmann/json.hpp>

namespace wdbib {

using std::string;
using std::unordered_map;
using std::vector;
using nlohmann::json;

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

struct WikiCiteItem
{
  string id;
  unordered_map<string, vector<string>> labels;
  unordered_map<string, string> descriptions;
  unordered_map<string, DataValue> claims;
};

// TODO
// void to_json(json& j, const WikiCiteItem& p) {
// }

void from_json(const json& j, WikiCiteItem& p);

}  // namespace wd

wd::WikiCiteItem ParseWikiciteJson(const string& id, const string& json_str);

/*
 * Performs a network request to Wikidata and downloads file.
 * Throws exceptions.
 */
std::pair<wd::WikiCiteItem, string> GetWikiciteItem(const string& qID);

string GetWikidataItems(const string& query_ids);
string GetWikidataItems(const vector<string>& q_ids);

}  // namespace wdbib
