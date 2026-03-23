// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef VOLTAGE_CONFIG_HPP__
#define VOLTAGE_CONFIG_HPP__

#include <trans_mode.hpp>

#include <string>
#include <string_view>
#include <vector>
#include <filesystem>
#include <memory>
#include <lane.hpp>

struct TestData
{
  TestData(std::string_view line, size_t linenum);
  // 1st line is 1
  size_t lineNumber;
  util::Lane lane;
  util::TransSpeed speed;
  util::TransMode d0mode;
  double d0InitVoltage;
  double d0TargetVoltage;
  util::TransMode d1mode;
  double d1InitVoltage;
  double d1TargetVoltage;

  double getD0InitVoltage() const {return d0InitVoltage;}
  double getD0TargetVoltage() const { return d0TargetVoltage; }
  double getD1InitVoltage() const { return d1InitVoltage < 0 ? d0InitVoltage : d1InitVoltage; }
  double getD1TargetVoltage() const { return d1TargetVoltage < 0 ? d0TargetVoltage : d1TargetVoltage; }
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

  const_iterator begin() const { return _sorted.cbegin(); }
  const_iterator end() const { return _sorted.cend(); }
  const TestData& operator[](size_t i) const { return *_sorted.at(i); }
  size_t length() const { return _sorted.size(); }

private:
  std::vector<std::unique_ptr<TestData>> _list;
  std::vector<TestData*> _sorted;
};

#endif
