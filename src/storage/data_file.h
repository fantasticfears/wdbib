#pragma once
#ifndef DATA_FILE_H_
#define DATA_FILE_H_

#include <fstream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <boost/core/noncopyable.hpp>
#include <boost/move/utility_core.hpp>

#include "../wikicite.h"

namespace wdbib {

// kDefaultDataFilename is the filename for storing uris
const char* const kDefaultDataFilename = "citation";

// kDefaultCachedDataFilename is the filename for storing cached wikidata
// information
const char* const kDefaultCachedDataFilename = "citation.lock";
const char* const kDefaultCachedDataExtrension = "lock";

/*
 * BibDataFile manages the data file and exposes streams to the user.
 */
class BibDataFile : private boost::noncopyable
{
 public:
  BibDataFile(const std::string& filename, const std::string& lock_ext);
  void LoadAll(std::function<void(std::ifstream&)> spec,
               std::function<void(std::ifstream&)> data) const;
  void LoadSpec(std::function<void(std::ifstream&)> spec) const;
  void LoadData(std::function<void(std::ifstream&)> data) const;
  void SaveAll(std::function<void(std::ofstream&)> spec,
               std::function<void(std::ofstream&)> data) const;
  void SaveSpec(std::function<void(std::ofstream&)> spec) const;
  void SaveData(std::function<void(std::ofstream&)> data) const;

 private:
  std::string spec_filename_;
  std::string data_filename_;
};

struct WdbibFileContent;
class DataFileContent;

namespace file {

std::unique_ptr<WdbibFileContent> LoadWdbibData(const BibDataFile& file);
void SaveWdbibData(const BibDataFile& file, const WdbibFileContent* content);

}  // namespace file

}  // namespace wdbib

#endif  // DATA_FILE_H_
