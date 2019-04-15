#pragma once

#include <memory>
#include <string>

namespace wdbib {

using std::string;

namespace file {

string ReadAll(const string& path);
void OverWrite(const string& path, const string& content);

} // namespace

/*
 * BibDataFile manages the data file.
 * It's aware of semantics of file.
 */
class BibDataFile
{
 public:
  explicit BibDataFile(const string& path);
 private:
  string path_;
};

}  // namespace wdbib
