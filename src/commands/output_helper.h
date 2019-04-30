#pragma once
#ifndef COMMANDS_OUTPUT_HELPER_H_
#define COMMANDS_OUTPUT_HELPER_H_

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
  function<void(SpinnerObject*)> callback;
};

const char* const kDotSpinner = "⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏";

class MultiLineSpinner
{
 public:
  MultiLineSpinner(int32_t pool_size, int32_t interval = 80)
      : interval_(interval), pool_size_(pool_size){};
  void register_append(const SpinnerObject& obj) { lines_.push_back(obj); }

  void LoopSpinner()
  {
    hideCursor();

    boost::asio::thread_pool pool(pool_size_);
    bool unfinished = true;
    int i = 0;
    char ch[4];
    while (unfinished) {
      unfinished = false;

      for (auto iter = lines_.begin(); iter != lines_.end(); ++iter) {
        switch (iter->status) {
        case SpinnerStatus::kPending:
          unfinished = true;
          cout << iter->pending_text;
          iter->status = SpinnerStatus::kLoading;
          break;
        case SpinnerStatus::kLoading:
          unfinished = true;
          static int len = strlen(kDotSpinner) / 3;

          i = (i >= (len - 1)) ? 0 : i + 1;
          strncpy(ch, kDotSpinner + i * 3, 3);
          cout << iter->pending_text << " " << ch;

          boost::asio::post(pool, [=]() {
            iter->callback(&(*iter));
            iter->status = SpinnerStatus::kFinished;
          });
          break;
        case SpinnerStatus::kFinished:
          cout << iter->finsihed_text;
          break;
        default:
          break;
        }
        if (std::next(iter) != lines_.cend()) {
          cout << endl;
        }
      }
      cout.flush();
      clearPrevLines(lines_.size());

      if (unfinished) {
        std::this_thread::sleep_for(std::chrono::milliseconds(interval_));
      }
    }
    showCursor();
    pool.join();

    for (auto iter = lines_.cbegin(); iter != lines_.cend(); ++iter) {
      cout << iter->finsihed_text << endl;
    }
  }

 private:
  vector<SpinnerObject> lines_;
  int32_t interval_;
  int32_t pool_size_;
  inline void hideCursor() { std::cout << "\u001b[?25l"; }
  inline void showCursor() { std::cout << "\u001b[?25h"; }
  inline void clearPrevLines(int32_t num_lines)
  {
    for (auto i = 0; i < num_lines - 1; ++i) {
      cout << "\033[F\033[J";
    }
  }  //, num_lines, "F", "\033[", num_lines, "J"); }
};

}  // namespace spinners

// PrintHighlight()

}  // namespace wdbib

#endif  // COMMANDS_OUTPUT_HELPER_H_
