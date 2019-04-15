#include "show.h"

#include <iostream>
#include <string>
#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#endif

#include <absl/strings/str_cat.h>

namespace wdbib {


void openURIInBrowser(const string& url)
{
#ifdef _WINDOWS
  ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
#else
  system(absl::StrCat("open ", url).c_str());
#endif
}

void ProcessShowQID(const string& qID, const CommandShowOption& show_option)
{
  using namespace std;

  if (!show_option.Legal()) {
    cerr << "wrong show options" << endl;
  }
  if (show_option.scholia) {
    openURIInBrowser(
        absl::StrCat("https://tools.wmflabs.org/scholia/work/", qID));
  } else if (show_option.wikidata) {
    openURIInBrowser(absl::StrCat("https://www.wikidata.org/wiki/", qID));
  }
}

}  // namespace wdbib
