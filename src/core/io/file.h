#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace io {

bool readFile(const char* path, std::vector<uint8_t>& out);
bool readText(const char* path, std::string& out);

} // namespace io
