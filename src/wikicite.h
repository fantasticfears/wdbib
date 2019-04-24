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

struct Snak
{
  DataValue dv;
  string snaktype;
};

struct WikidataItem
{
  string id;
  unordered_map<string, vector<string>> labels;
  unordered_map<string, vector<string>> descriptions;
  unordered_map<string, Snak> claims;
};

void from_json(const json& j, Snak& snak);
void from_json(const json& j, WikidataItem& p);

}  // namespace wd

enum class WikiCiteItemType
{
  kArticle,
  kBook,
  kMisc,
  kWebpage
};

enum class WikidataProperty
{
  kInstanceOf,
  kAuthor,
  kDirector,
  kComposer,
  kEditor,
  kIllustrator,
  kPublisher,
  kGenre,
  kISBN,
  kISSN,
  kPublisherPlace,
  kPage,
  kVersion,
  kDOI,
  kEdition,
  kIssue,
  kVolume,
  kIssued,
  kTranslator,
  kPMID,
  kPMCID,
  kURL,
  kNnumberOfPages,
  kContainerTitle,
  kTitle
};

struct WikiCiteItem
{
  string qid;
  WikiCiteItemType instance_of;
  string title;
  string subject;
};

struct WdbibFileContent;
std::string JsonToBibTex(const nlohmann::json& j, const WdbibFileContent* content);
WikiCiteItem WikidataToWikicite(const wd::WikidataItem& item);

}  // namespace wdbib

#endif  // WIKICITE_H_