// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include <bitstream.hpp>
#include <iostream>
namespace util
{

static bool startsWith(
  std::string_view str,
  std::string_view start) {
    if (str.size() < start.size()) return false;
    return std::equal(start.begin(), start.end(), str.begin());
}

std::filesystem::path findFile(
  const std::filesystem::path& directory,
  std::string_view startName,
  std::string_view extName
) {
  using namespace std::filesystem;
  std::string extName2;
  std::string_view ext = extName;
  if (!extName.empty() && extName[0] != '.') {
    extName2.reserve(1 + extName.length());
    extName2 = ".";
    extName2 += extName;
    ext = extName2;
  }


  for (const auto& e: directory_iterator(directory)) {
    if (!e.is_regular_file()) continue;
    const auto p = e.path();
    if (p.extension() == ext) {
      if (startsWith(e.path().filename().generic_u8string(), startName)) {
        return p;
      }
    }
  }

  return "";
}

}
