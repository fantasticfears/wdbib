#include "meta.h"

#include <iostream>
#include <string>
#include <string_view>
#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#endif

#include <absl/strings/str_cat.h>

#include "validators.h"

namespace wdbib {

void openURIInBrowser(const std::string& url)
{
#ifdef _WINDOWS
  ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
#else
  system(absl::StrCat("open ", url).c_str());
#endif
}

namespace detail {

const char* const kTryMessage = "\n\nMaybe try:\n  wdbib view Q26720266 --scholia\n";
const char* const kIDInvalidOnViewMessage = "it's not a valid Q-identifier.\n\nMaybe try:\n  wdbib view Q26720266 --scholia\n";

} // namespace detail

const static QIDValidator gQIDViewValidator(detail::kIDInvalidOnViewMessage);

void SetupViewSubCommand(CLI::App& app)
{
  using namespace std;

  auto cmd = app.add_subcommand("view", "View a bibliographical item.");
  auto opt = make_shared<ViewSubCmdOpt>();

  auto view_wd =
      cmd->add_flag("-w,--wikidata", opt->wikidata, "Show item on Wikidata.");
  auto view_scholia =
      cmd->add_flag("-s,--scholia", opt->scholia, "Show item on Scholia.");
  view_scholia->excludes(view_wd);
  view_wd->excludes(view_scholia);
  auto qid_opt = cmd->add_option("QID", opt->qid, "Q-id to view");
  qid_opt->required()->check(gQIDViewValidator);
  cmd->failure_message([](const CLI::App* app, const CLI::Error& e) {
    // auto header = CLI::FailureMessage::simple(app, e);

    string header;
    header += detail::kTryMessage;

    return header;
  });
  cmd->callback([opt]() { RunViewSubCommand(*opt); });
}

void RunViewSubCommand(const ViewSubCmdOpt& opt)
{
  using namespace std;

  if (!(opt.scholia || opt.wikidata)) {
    throw CLI::ValidationError(absl::StrCat("A view option is needed.", detail::kTryMessage));
  }

  if (opt.scholia) {
    openURIInBrowser(
        absl::StrCat("https://tools.wmflabs.org/scholia/work/", opt.qid));
  } else if (opt.wikidata) {
    openURIInBrowser(absl::StrCat("https://www.wikidata.org/wiki/", opt.qid));
  }
}

}  // namespace wdbib
