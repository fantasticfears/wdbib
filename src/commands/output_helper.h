#pragma once

#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <thread>
#include <vector>

#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>

namespace wdbib {

namespace spinners {

using std::cout;
using std::endl;
using std::function;
using std::string;
using std::vector;

enum class SpinnerStatus
{
  kPending,
  kLoading,
  kFinished
};
struct SpinnerObject
{
  SpinnerStatus status;
  string pending_text;
  string finsihed_text;
  function<void(void)> callback;
  void Start() { status = SpinnerStatus::kLoading; }
};

const char* const kDotSpinner = "⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏";

class MultiLineSpinner
{
 public:
  MultiLineSpinner(const string& loading_text, int32_t pool_size,
                   int32_t interval = 80)
      : loading_text_(loading_text), interval_(interval), pool_(pool_size){};
  ~MultiLineSpinner() { pool_.join(); }
  SpinnerObject* append(SpinnerObject&& obj)
  {
    lines_.push_back(obj);
    return &lines_.back();
  }

  void LoopSpinner()
  {
    hideCursor();

    bool unfinished = true;
    while (unfinished) {
      unfinished = false;

      for (auto& l : lines_) {
        switch (l.status) {
        case SpinnerStatus::kPending:
          unfinished = true;
          cout << l.pending_text << endl;
          break;
        case SpinnerStatus::kLoading:
          unfinished = true;
          static int len = strlen(kDotSpinner) / 3;
          int i = 0;
          char ch[4];

          i = (i >= (len - 1)) ? 0 : i + 1;
          strncpy(ch, kDotSpinner + i * 3, 3);
          cout << ch << " " << loading_text_ << " \r";

          boost::asio::post(pool_, [&]() {
            l.callback();
            l.status = SpinnerStatus::kFinished;
          });
          break;
        case SpinnerStatus::kFinished:
          cout << l.finsihed_text << endl;
          break;
        default:
          break;
        }
      }
      cout.flush();

      std::this_thread::sleep_for(std::chrono::milliseconds(interval_));
    }
    showCursor();
  }

 private:
  int interval;
  string loading_text_;
  vector<SpinnerObject> lines_;
  int32_t interval_;
  boost::asio::thread_pool pool_;
  void hideCursor() { std::cout << "\u001b[?25l"; }
  void showCursor() { std::cout << "\u001b[?25h"; }
};

}  // namespace spinners

// PrintHighlight()

}  // namespace wdbib
