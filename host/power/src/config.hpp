// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef POWER_CONFIG_HPP__
#define POWER_CONFIG_HPP__

#include <string>
#include <string_view>
#include <vector>
#include <filesystem>
#include <memory>

#include <trans_mode.hpp>

struct TestData
{
  TestData(std::string_view line, size_t linenum);
  // 1st line is 1
  std::string text;
  size_t lineNumber;
  util::TransSpeed speed;

  double txInitVoltage;
  double txLastVoltage;
  double rxInitVoltage;
  double rxLastVoltage;
  size_t step;

  std::vector<uint64_t> params;
};

bool operator<(const TestData& left, const TestData& right);
bool operator<=(const TestData& left, const TestData& right);
bool operator>(const TestData& left, const TestData& right);
bool operator>=(const TestData& left, const TestData& right);


class TestConfig
{
public:
  using const_iterator = std::vector<TestData*>::const_iterator;
  TestConfig(const std::filesystem::path& path);

  const_iterator begin() const { return _ptrs.cbegin(); }
  const_iterator end() const { return _ptrs.cend(); }
  const TestData& operator[](size_t i) const { return *_ptrs.at(i); }
  size_t length() const { return _ptrs.size(); }

private:
  std::vector<std::unique_ptr<TestData>> _list;
  std::vector<TestData*> _ptrs;
};

#endif
