#ifndef XSDB_ERROR__HPP__
#define XSDB_ERROR__HPP__

#include <exception>
#include <stdexcept>

namespace xsdb {

class XsdbError: public std::runtime_error
{
public:
  XsdbError(const char* msg): std::runtime_error(msg), timeout(false) {}
  XsdbError(const std::string& msg): std::runtime_error(msg), timeout(false) {}
  XsdbError(const std::string& msg, bool timeout)
    : std::runtime_error(msg), timeout(timeout)
  {
  }

  bool timeout;
};


}

#endif
