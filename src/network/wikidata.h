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

void from_json(const json& j, WikiCiteItem& p)
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

wd::WikiCiteItem ParseWikidataJson(string id, string json_str)
{
  using nlohmann::json;

  auto t = json::parse(json_str);
  return t.at("entities").at(id).get<wd::WikiCiteItem>();
}

}  // namespace wdbib
