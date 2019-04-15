#pragma once

#include "../network/wikidata.h"

#include <iostream>

namespace wdbib {

void ProcessAddQIDs(const vector<string>& qIDs);

void ProcessAddQID(const string& qID);

} // namespace wdbib
