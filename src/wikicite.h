#pragma once

#include <string>

namespace wdbib {

using namespace std;

enum class WikiCiteItemType {
  kArticle,
};

struct WikiCiteItem {
  string qid;
  WikiCiteItemType instance_of;
  string title;
  string subject;
};

} // namespace wdbib