#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <nlohmann/json.hpp>

namespace wdbib {

using std::string;
using std::unordered_map;
using std::vector;

namespace wd {

using nlohmann::json;

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
wd::WikiCiteItem GetWikiciteItem(const string& qID);

}  // namespace wdbib
