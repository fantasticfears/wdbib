#include <memory>
#include <string>

namespace wdbib {

using std::string;

namespace file {

string ReadAll(const string& path);
void OverWrite(const string& path, const string& content);

} // namespace

class BibDataFile
{
 public:
  explicit BibDataFile(const string& path);
 private:
  string path_;
};

}  // namespace wdbib
