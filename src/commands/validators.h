#pragma once

#include <string>

#include "CLI/Validators.hpp"

namespace wdbib {

struct QIDValidator : public CLI::Validator
{
  QIDValidator(const std::string& message)
  {
    tname = "QID";
    func = [=](const std::string& str) {
      if (!(str.size() > 0 && str[0] == 'Q')) {
        return message;
      } else {
        return std::string();
      }
    };
  }
};


}  // namespace wdbib
