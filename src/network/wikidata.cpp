#include "wikidata.h"

#include <absl/strings/str_cat.h>
#include <absl/strings/str_join.h>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include "spdlog/spdlog.h"

#include "../system.h"

namespace wdbib {

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

wd::WikidataItem ParseWikiciteJson(const string& id, const string& json_str)
{
  using nlohmann::json;

  auto t = json::parse(json_str);
  return t.at("entities").at(id).get<wd::WikidataItem>();
}

using tcp = boost::asio::ip::tcp;     // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;  // from <boost/beast/http.hpp>
namespace ssl = boost::asio::ssl;

const char* const kWikidataHost = "www.wikidata.org";
const char* const kWikidataPort = "443";
const int kHttpVersion = 11;

string GetWikidataItems(const string& query_ids)
{
  boost::asio::io_context ioc;
  ssl::context ctx(ssl::context::method::tls_client);
  ssl::stream<tcp::socket> ssock(ioc, ctx);

  tcp::resolver resolver(ioc);
  auto it = resolver.resolve({kWikidataHost, kWikidataPort});

  boost::asio::connect(ssock.lowest_layer(), it);
  ssock.handshake(ssl::stream_base::handshake_type::client);

  string target = absl::StrCat("/w/api.php?action=wbgetentities&format=json&ids=", query_ids);
  http::request<http::string_body> req{http::verb::get, target, kHttpVersion};
  req.set(http::field::host, kWikidataHost);
  static auto ua = GetUserAgent();
  req.set(http::field::user_agent, ua);

  spdlog::get("network")->debug("requesting: " + target);

  http::write(ssock, req);

  boost::beast::flat_buffer buffer;
  http::response<http::dynamic_body> res;
  http::read(ssock, buffer, res);
  spdlog::get("network")->debug("requesting: " + boost::beast::buffers_to_string(res.body().data()));

  return boost::beast::buffers_to_string(res.body().data());
}

string GetWikidataItems(const vector<string>& q_ids)
{
  return GetWikidataItems(absl::StrJoin(q_ids, "|"));
}


std::pair<wd::WikidataItem, string> GetWikiciteItem(const string& qID)
{
  string item_json = GetWikidataItems(qID);
  return std::make_pair(ParseWikiciteJson(qID, item_json), item_json);
}

}  // namespace wdbib
