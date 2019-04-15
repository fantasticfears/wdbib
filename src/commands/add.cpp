#include "meta.h"

#include <algorithm>
#include <iostream>
#include <memory>

#include <fmt/format.h>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>

#include "../network/wikidata.h"
#include "../storage/data_file.h"
#include "output_helper.h"

namespace wdbib {

void SetupAddSubCommand(CLI::App& app)
{
  auto cmd = app.add_subcommand("add", "add a citation to the library");
  auto opt = std::make_shared<AddSubCmdOpt>();

  cmd->add_option("-q,--qid", opt->qids,
                      "citation Q-identifier on Wikidata");
  cmd->callback([opt]() { RunAddSubCommand(*opt); });
}

void ProcessAddQID(const string& qid)
{
  auto item = GetWikiciteItem(qid);
  file::OverWrite("citation", fmt::format("wc:{0}:{1}", item.id,
                                          item.labels["en"].front()));
}

constexpr size_t kMaxThreadPoolSize = 8;

void RunAddSubCommand(const AddSubCmdOpt& opt)
{
  using namespace std;

  auto pool_size = std::max(opt.qids.size(), kMaxThreadPoolSize);
  boost::asio::thread_pool pool(pool_size);

  for (const auto& qid : opt.qids) {
    spinners::Spinner spinner("Adding " + qid);
    boost::asio::post(pool, [&]() {
      ProcessAddQID(qid);
      spinner.stop();
      cout << "Added " + qid << endl;
    });
    spinner.startSpinner();
  }

  pool.join();

}

}  // namespace wdbib
