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
#include "../wikicite.h"

namespace wdbib {

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
