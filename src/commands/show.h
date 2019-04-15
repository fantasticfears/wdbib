#pragma once

#include <string>

namespace wdbib {

using std::string;

struct CommandShowOption {
  bool wikidata = false;
  bool scholia = false;
  bool Legal() const {
    return !(wikidata && scholia) && (wikidata || scholia);
  }
};

void ProcessShowQID(const string& qID, const CommandShowOption& show_option);

} // namespace wdbib
