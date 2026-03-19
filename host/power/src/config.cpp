#include "./config.hpp"

#include <sstream>
#include <charconv>
#include <stdexcept>
#include <fstream>

#include <iostream>
#include <algorithm>
#include <parse_double.hpp>

static constexpr const char* WHITE_SPACE = " \t\r\n";
static constexpr const char* SEP = ",";
static constexpr auto npos = std::string_view::npos;


bool operator<(const TestData& left, const TestData& right)
{
  if (left.speed.speed == right.speed.speed)
    return left.lineNumber < right.lineNumber;
  return left.speed.speed < right.speed.speed;
}

bool operator<=(const TestData& left, const TestData& right)
{
  if (left.speed.speed == right.speed.speed)
    return left.lineNumber <= right.lineNumber;

  return left.speed.speed <= right.speed.speed;
}

bool operator>(const TestData& left, const TestData& right)
{
  if (left.speed.speed == right.speed.speed)
    return left.lineNumber > right.lineNumber;

  return left.speed.speed > right.speed.speed;
}

bool operator>=(const TestData& left, const TestData& right)
{
  if (left.speed.speed == right.speed.speed)
    return left.lineNumber >= right.lineNumber;

  return left.speed.speed >= right.speed.speed;
}

TestData::TestData(std::string_view line, size_t linenum):
  text(line),
  lineNumber(linenum),
  speed(),
  params()
{
  size_t pos = 0;
  size_t i = 0;
  while (pos != npos) {
    pos = line.find_first_not_of(WHITE_SPACE, pos);
    const auto from = pos;
    pos = line.find_first_of(SEP, pos);

    size_t len = npos;
    if (pos != npos) {
      size_t end = line.find_last_not_of(WHITE_SPACE, pos - 1);
      len = end - from + 1;
      pos = pos + 1;
    }
    std::string_view data = line.substr(from, len);


    switch (i) {
    case 0: // 1nd data is Transceiver speed.
      speed = util::TransSpeed(data);
      break;

      // voltage config
    case 1:
    case 2:
    case 3:
    case 4:
    {
      if (len == 0) {
        std::stringstream ss;
        ss << "Fail to parse voltage: " << data;
        throw std::runtime_error(ss.str());
      }
      if (data[len - 1] == 'V' || data[len - 1] == 'v') {
        data = data.substr(0, len - 1);
        size_t end = data.find_last_not_of(WHITE_SPACE);
        if (end != npos)
          data = data.substr(0, end + 1);
      }
      auto ret = parse_double(data);
      if (!ret.ok && ret.end != data.cend()) {
        std::stringstream ss;
        ss << "Illegal voltage: " << data;
        throw std::runtime_error(ss.str());
      }

      if (ret.value <= 0) {
        std::stringstream ss;
        ss << "Fail to parse voltage: " << data;
        throw std::runtime_error(ss.str());
      }

      switch (i) {
      case 1: txInitVoltage = ret.value; break;
      case 2: txLastVoltage = ret.value; break;
      case 3: rxInitVoltage = ret.value; break;
      case 4: rxLastVoltage = ret.value; break;
      }
    }
    break;

    default:
    {
      const auto e = data.cend();
      uint64_t x;
      const auto ret = std::from_chars(data.cbegin(), e, x);
      if (ret.ec != std::errc{}) {
        std::stringstream ss;
        ss << "Fail to parse number: " << data;
        throw std::runtime_error(ss.str());
      }
      if (i == 5) step = x;
      else params.push_back(x);
    }
    break;
    }// end switch
    i++;
  }

  if (i < 3) {//TODO: WIP max parameter size?
    std::stringstream ss;
    ss << "Not enough data: " << line;
    throw std::runtime_error(ss.str());
  }
}


TestConfig::TestConfig(const std::filesystem::path& path)
{
  std::ifstream f(path);
  if (!f) {
    std::stringstream ss;
    ss << "Fail to open file: " << path;
    throw std::runtime_error(ss.str());
  }


  size_t lineNum = 0;
  try {
    std::string line;
    while (std::getline(f, line)) {
      size_t f = line.find_first_not_of(WHITE_SPACE, 0);
      size_t e = line.find_last_not_of(WHITE_SPACE);

      if (f != e && line[f] != '#') {
        const std::string_view l(&line[f], e - f + 1);
        const auto v = _list.emplace_back(std::make_unique<TestData>(l, lineNum + 1)).get();
        _ptrs.emplace_back(v);
      }
      lineNum++;
    }
  }
  catch (std::runtime_error& e) {
    std::stringstream ss;
    ss << "Fail to read configuration file:" << path << ":" << (lineNum + 1) << std::endl
      << e.what();
    throw std::runtime_error(ss.str());
  }
}


