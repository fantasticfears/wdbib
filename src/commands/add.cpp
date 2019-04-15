#include "add.h"

#include <algorithm>
#include <iostream>

#include <fmt/format.h>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>

#include "../storage/data_file.h"
#include "output_helper.h"

namespace wdbib {

void ProcessAddQID(const string& qID)
{
  auto item = GetWikiciteItem(qID);
  file::OverWrite("citation", fmt::format("wc:{0}:{1}", item.id,
                                          item.labels["en"].front()));
}

constexpr size_t kMaxThreadPoolSize = 8;

void ProcessAddQIDs(const vector<string>& qIDs)
{
  using namespace std;

  auto pool_size = std::max(qIDs.size(), kMaxThreadPoolSize);
  boost::asio::thread_pool pool(pool_size);

  for (const auto& qID : qIDs) {
    spinners::Spinner spinner("Adding " + qID);
    boost::asio::post(pool, [&]() {
      ProcessAddQID(qID);
      spinner.stop();
      cout << "Added " + qID << endl;
    });
    spinner.startSpinner();
  }

  pool.join();
}

}  // namespace wdbib
