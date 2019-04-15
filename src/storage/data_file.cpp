#include "data_file.h"

#include <fstream>

namespace wdbib {

namespace file {

using namespace std;

const int kFileEndOffset = 0;
const int kFileStartOffset = 0;

string ReadAll(const string& path)
{
  ifstream fs(path);
  fs.seekg(kFileEndOffset, ios::end);
  auto size = fs.tellg();
  string buffer(size, ' ');
  fs.seekg(kFileStartOffset);
  fs.read(&buffer[0], size);
  return buffer;
}

void OverWrite(const string& path, const string& content)
{
  ofstream fs(path, ofstream::out | ofstream::trunc);
  fs.write(content.c_str(), content.size());
}

}  // namespace file

// TODO: validate path
BibDataFile::BibDataFile(const string& path) : path_(path) {}


}  // namespace wdbib
