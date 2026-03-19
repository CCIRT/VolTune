#include <xsdb.hpp>
#include <iostream>

int main(int argc, char const* argv[])
{
  xsdb::Xsdb x(nullptr);
  x.setTimeout(2000);
  std::cout << "connect" << std::endl;
  x.connect();
  std::cout << "targets" << std::endl;
  const auto ret = x.targets();
  std::cout << ret;
  return 0;
}

