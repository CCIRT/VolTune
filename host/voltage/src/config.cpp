#include "./config.hpp"
#include <parse_double.hpp>

#include <sstream>
#include <charconv>
#include <stdexcept>
#include <fstream>

#include <iostream>

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
  lineNumber(linenum),
  lane(util::Lane::Unknown),
  speed(),
  d0mode(util::TransMode::TX),
  d0InitVoltage(-1),
  d0TargetVoltage(-1),
  d1mode(util::TransMode::RX),
  d1InitVoltage(-1),
  d1TargetVoltage(-1)
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
    const auto e = data.cend();

    switch (i) {
    case 0: // 1st data is lane name
      lane = util::getLane(data);
      if (lane == util::Lane::Unknown) {
        std::stringstream ss;
        ss << "[Line " << linenum << "] Unknown lane name: " << data;
        throw std::runtime_error(ss.str());
      }
      break;
    case 1: // 2nd data is Transceiver speed.
      speed = util::TransSpeed(data);
      break;
    case 2: // Tx, Rx, Loop, -
    case 5:
      util::TransMode m;
      util::TransMode r;
      if (data == "Tx" || data == "tx") {
        m = util::TransMode::TX;
        r = util::TransMode::RX;
      } else if (data == "Rx" || data == "rx") {
        m = util::TransMode::RX;
        r = util::TransMode::TX;
      } else if (data == "Loop" || data == "loop") {
        m = util::TransMode::LOOP;
        r = util::TransMode::LOOP;
      } else if (data == "-" || data == "--") {
        if (speed.speed != 0) {
          std::stringstream ss;
          ss << "Illegal mode: " << line;
          throw std::runtime_error(ss.str());
        }
        i++;
        continue;
      }
      if (i == 2) {
        d0mode = m;
        d1mode = r;
      } else if (i == 5) {
        if (m != d1mode) {
          std::stringstream ss;
          ss << "Illegal device2 mode: " << line;
          throw std::runtime_error(ss.str());
        }
      }
      break;

    default:
      const auto ret = parse_double(data);
      if (ret.end != e) {
        std::stringstream ss;
        ss << "Fail to parse number: " << data;
        throw std::runtime_error(ss.str());
      }
      double d = ret.value;
      if (d < 0) {
        std::stringstream ss;
        ss << "Invalid voltage: " << data;
        throw std::runtime_error(ss.str());
      }

      switch (i) {
      case 3: d0InitVoltage = d; break;
      case 4: d0TargetVoltage = d; break;
      case 6: d1InitVoltage = d; break;
      case 7: d1TargetVoltage = d; break;
      default:
      {
        std::stringstream ss;
        ss << "Illegal parameters: " << line;
        throw std::runtime_error(ss.str());
      }
      }
    }
    i++;
  }

  if (i < 4) {
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
        _sorted.emplace_back(v);
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

  std::sort(_sorted.begin(), _sorted.end(), [](auto x, auto y) {
    return *x < *y;
    });
}


