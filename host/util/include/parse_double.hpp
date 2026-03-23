// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef HOST_PARSE_DOUBLE_HPP__
#define HOST_PARSE_DOUBLE_HPP__

#include <iostream>

#define DISABLE_FROM_CHARS
#ifdef __GNUC__
# if __GNUC__ > 10
#  undef DISABLE_FROM_CHARS
# else
# include <features.h>
# endif
#endif

#include <string>
#include <string_view>

#ifndef DISABLE_FROM_CHARS
#  include <charconv>
#endif

struct ParseDoubleResult {
  bool ok;
  double value;
  const char* end;
};

inline ParseDoubleResult parse_double(std::string_view s)
{
#ifdef DISABLE_FROM_CHARS
  const std::string str(s);
  size_t index;
  double d = std::stod(str, &index);
  return { index != 0, d, &s[index] };
#else
  const auto e = s.cend();
  double d;
  const auto ret = std::from_chars(s.cbegin(), e, d);
  return { ret.ec == std::errc{} , d, ret.ptr };
#endif
}

#endif
