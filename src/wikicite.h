#pragma once
#ifndef WIKICITE_H_
#define WIKICITE_H_

#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>

namespace wdbib {

using namespace std;
using json = nlohmann::json;

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

// simplified
struct Snak
{
  DataValue dv;
  string snaktype;
};

struct WikidataItem
{
  string id;
  // simplified: aliases are merged to labels
  unordered_map<string, vector<string>> labels;
  unordered_map<string, vector<string>> descriptions;
  // simplified: only includes one `mainsnak`!
  unordered_map<string, Snak> claims;
};

void from_json(const json& j, Snak& snak);
void from_json(const json& j, WikidataItem& p);

}  // namespace wd

enum class WikiCiteItemType
{
#include "_wikicite_types_enum.gdh"
};

enum class WikidataProperty
{
#include "_wikicite_props_enum.gdh"
};

struct WikiCiteItem
{
  string qid;
  WikiCiteItemType instance_of;
  string title;
  string subject;
};

struct WdbibFileContent;
wd::WikidataItem TryParseWikidataJson(const nlohmann::json& j, const std::string qid);
std::string JsonToBibTex(const nlohmann::json& j, const WdbibFileContent* content);
WikiCiteItem WikidataToWikicite(const wd::WikidataItem& item);

}  // namespace wdbib

#endif  // WIKICITE_H_