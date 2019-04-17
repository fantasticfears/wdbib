#include "meta.h"

#include <cstdlib>
#include <fstream>
#include <regex>

#include "../storage/data_file.h"

namespace wdbib {

using namespace std;

void SetupConvertSubCommand(CLI::App& app)
{
  auto cmd = app.add_subcommand("convert", "convert citation data to bibtex file.");
  ConvertSubCmdOpt opt;
  auto t = cmd->add_option("-t,--to-file", opt.output_path, "the path for file to be written");
  t->required();

  cmd->callback([&opt]() { RunConvertSubCommand(opt); });
}

void RunConvertSubCommand(const ConvertSubCmdOpt& opt)
{
  BibDataLockFile lock(kDefaultCachedDataFilename);
  auto items = lock.All();

  ofstream out(opt.output_path);
  regex header_regex("([^\\s\\{]+\\{)",
      std::regex_constants::ECMAScript | std::regex_constants::icase);

  for (const auto& kv : items) {
    {
      ofstream tmp(".tmp.json");
      json t{};
      t["entities"] = json();
      t["entities"][kv.first] = kv.second;
      tmp << t.dump();
    }
    system("citation-js -i .tmp.json -o .tmp -f string -s bibtex");
    {
      ifstream tmp(".tmp.bib");
      string line;
      getline(tmp, line);
      auto begin = 
        std::sregex_iterator(line.begin(), line.end(), header_regex);
        auto end = std::sregex_iterator();
      if (begin != end) {
        out << begin->str() << kv.first << ',' << endl;
      } else {
        continue;
      }
      while (getline(tmp, line)) {
        out << line << endl;
      }
    }
    remove(".tmp.json");
    remove(".tmp.bib");
  }
}

}  // namespace wdbib
