#pragma once
#ifndef WIKICITE_H_
#define WIKICITE_H_

#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>

namespace wdbib {

using namespace std;

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
  kISBN,
  kNnumberOfPages,
  kContainerTitle,
  kTitle,
  kAuthor
}

struct WikiCiteItem
{
  string qid;
  WikiCiteItemType instance_of;
  string title;
  string subject;
};

std::string JsonToBibTex(const nlohmann::json& j);
WikiCiteItem WikidataToWikicite(const wd::WikidataItem& item);

}  // namespace wdbib

#endif  // WIKICITE_H_