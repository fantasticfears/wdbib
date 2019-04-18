

class BibDataLockFile
{
 public:
  explicit BibDataLockFile(const string& path) : path_(path)
  {
    std::ifstream i(path_);
    try {
      data_ = json::parse(i);
    } catch (...) {
      data_ = json();
    }
  }
  void update(const string& resp)
  {
    auto r = json::parse(resp);
    for (auto& [key, value] : r.at("entities").items()) {
      data_[key] = value;
    }
  }

  unordered_map<string, json> All() const
  {
    unordered_map<string, json> res;
    for (auto& [key, value] : data_.items()) {
      res[key] = value;
    }
    return res;
  }

  bool Found(const string& qid) { return data_.find(qid) != data_.end(); }

  void Save()
  {
    std::ofstream f(path_, std::ios::out | std::ios::trunc);
    f << data_.dump() << std::endl;
  }

 private:
  string path_;
  json data_;
};