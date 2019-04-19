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

struct WikidataItem;

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
