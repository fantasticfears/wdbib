#pragma once
#ifndef WIKICITE_H_
#define WIKICITE_H_

#include <string>
#include <unordered_map>

namespace wdbib {

using namespace std;


enum class WikiCiteItemType
{
  kArticle,
  kBook,
  kMisc,
  kWebpage
};

const unordered_map<string, WikiCiteItemType> gkWikiciteProps = {
#include "_wikicite_types.gdh"
};

struct WikiCiteItem
{
  string qid;
  WikiCiteItemType instance_of;
  string title;
  string subject;
};

}  // namespace wdbib

#endif  // WIKICITE_H_