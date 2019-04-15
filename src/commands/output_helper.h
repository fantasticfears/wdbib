#pragma once

#include <chrono>
#include <iostream>
#include <map>
#include <sstream>
#include <thread>

namespace wdbib {

namespace spinners {

const char *const kDotSpinner = "⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏";

class Spinner
{
 public:
  Spinner(std::string _text)
      : interval(80), text(_text), stop_spinner(false), symbols(kDotSpinner){};
  Spinner() : interval(), text(""), stop_spinner(false), symbols(kDotSpinner){};
  Spinner(int _interval, std::string _text, const char *_symbols)
      : interval(_interval),
        text(_text),
        stop_spinner(false),
        symbols(kDotSpinner){};
  ~Spinner() { stop(); };

  void setInterval(int _interval) { interval = _interval; }
  void setText(std::string _text) { text = _text; }

  void startSpinner()
  {
    int len = strlen(symbols) / 3;
    int i = 0;
    char ch[4];

    hideCursor();
    while (!stop_spinner) {
      i = (i >= (len - 1)) ? 0 : i + 1;
      strncpy(ch, symbols + i * 3, 3);
      std::cout << ch << " " << text << " \r";
      std::cout.flush();
      std::this_thread::sleep_for(std::chrono::milliseconds(80));
    }

    showCursor();
  }

  void start() { t = std::thread(&Spinner::startSpinner, this); }

  void stop()
  {
    stop_spinner = true;
    if (t.joinable()) {
      t.join();
    }
  }

 private:
  int interval;
  std::string text;
  bool stop_spinner;
  const char *symbols;
  std::thread t;

  void hideCursor() { std::cout << "\u001b[?25l"; }

  void showCursor() { std::cout << "\u001b[?25h"; }
};
}  // namespace spinners

// PrintHighlight()

}  // namespace wdbib
