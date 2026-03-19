#include "xsdb.hpp"
#include <chrono>
#include <functional>
#include <sstream>
#include <string.h>
#include <regex>
#include <iostream>

#ifndef _WIN32
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace ch = std::chrono;

namespace xsdb {

static std::regex TargetLineRegex(R"(( +)(\d+)(\*?) +(.*))");

TargetDevice::TargetDevice(const std::string& text):
  jtag(0),
  name(),
  selected(false),
  indent(0),
  sub(),
  parent(nullptr)
{
  std::smatch m;
  if (std::regex_match(text, m, TargetLineRegex)) {
    indent = m[1].length();
    jtag = std::stoul(m[2].str());
    selected = m[3].length() != 0;
    name = m[4].str();
  }
}

void Targets::add(const std::string& line) {
  TargetDevice d(line);
  if (d.jtag == 0) return;

  while (_parent && _parent->indent >= d.indent)
    _parent = _parent->parent;

  if (_parent == nullptr) {
    _parent = targets.emplace_back(std::make_unique<TargetDevice>(std::move(d))).get();
  } else {
    d.parent = _parent;
    _parent = _parent->sub.emplace_back(std::make_unique<TargetDevice>(std::move(d))).get();
  }
}


template <typename T> static bool _isTimeout(const T& start, uint64_t timeout)
{
  if (timeout == 0)
    return false;
  const auto time = ch::system_clock::now() - start;
  const auto mcount = ch::duration_cast<ch::milliseconds>(time).count();
  if (mcount < 0)
    return false;
  return timeout < (size_t)mcount;
}



static constexpr size_t START_XSDB_TIMEOUT =
# ifdef FIRST_TIMEOUT
FIRST_TIMEOUT;
# else
(10 * 1000);
# endif

#ifdef _WIN32

// --------------------------
// for Windows
// --------------------------

static constexpr WCHAR CMD[] = L"cmd.exe /C ";
static constexpr size_t CMD_LEN = sizeof(CMD) / sizeof(WCHAR) - 1; // *note* -1: Null charactor
static constexpr WCHAR XSDB_BAT[] = L"xsdb.bat";

static bool fileExists(LPCWSTR path)
{
  const auto attr = GetFileAttributesW(path);
  return (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY));
}

static bool _findXSDB(LPCWSTR dir, LPCWSTR pattern, WCHAR* buffer)
{
  WIN32_FIND_DATAW ffd;
  HANDLE h = FindFirstFileW(pattern, &ffd);
  if (h == INVALID_HANDLE_VALUE) return false;

  const auto dirlen = wcslen(dir) + 1;
  wmemcpy(buffer, dir, dirlen - 1);
  buffer[dirlen - 1] = '\\';

  constexpr WCHAR xsdb[] = L"\\bin\\xsdb.bat";

  do {
    if (wcscmp(ffd.cFileName, L".") == 0 || wcscmp(ffd.cFileName, L"..") == 0)
      continue;
    if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      const auto len = wcslen(ffd.cFileName);
      wmemcpy(buffer + dirlen, ffd.cFileName, len);
      wcscpy(buffer + dirlen + len, xsdb);
      if (fileExists(buffer)) return true;
    }
  } while (FindNextFileW(h, &ffd) != 0);

  FindClose(h);
  return false;
}

static bool findXSDB(WCHAR* buffer)
{
  // find 2022.1
  constexpr WCHAR xsdb1[] = L"C:\\Xilinx\\Vivado\\2022.1\\bin\\xsdb.bat";
  constexpr WCHAR xsdb2[] = L"C:\\Xilinx\\Vivado_Lab\\2022.1\\bin\\xsdb.bat";
  if (fileExists(xsdb1)) {
    wcscpy(buffer, xsdb1);
    return true;
  }

  if (fileExists(xsdb2)) {
    wcscpy(buffer, xsdb2);
    return true;
  }

  // find other version
  if (_findXSDB(L"C:\\Xilinx\\Vivado", L"C:\\Xilinx\\Vivado\\*", buffer)) {
    return true;
  }
  if (_findXSDB(L"C:\\Xilinx\\Vivado_Lab", L"C:\\Xilinx\\Vivado_Lab\\*", buffer)) {
    return true;
  }
  return false;
}

#else

// --------------------------
// Constants for Linux
// --------------------------

static constexpr int PATH_SIZE = 4096;
#endif


static constexpr int READ = 0;
static constexpr int WRITE = 1;
Xsdb::Xsdb(const char* xsdb)
  : _closed(false)
  , _stdin()
  , _stdout()
  , _stderr()
#ifdef _WIN32
  , _pi()
#else
  , _pid(0)
#endif
  , _buffer()            //
  , _bufsize(0)          //
  , _ebuffer()           //
  , _ebufsize(0)         //
  , _timeout(0)          //
  , _connected(false)   //
  , _target(0)           //
  , _log(false) //
{
  static constexpr int PATH_SIZE = 4096;
  if (xsdb != nullptr) {
    auto len = strlen(xsdb);
    if (len >= PATH_SIZE) {
      std::stringstream ss;
      ss << "Too long xsdb path:" << xsdb;
      throw std::invalid_argument(ss.str());
    } else if (len == 0) {
      xsdb = nullptr;
    }
  }

# ifdef _WIN32
  // --------------------------
  //  Windows
  // --------------------------


  struct wc {
    WCHAR* ptr = nullptr;
    ~wc() {
      if (ptr) delete[] ptr;
    }
    void init(size_t len) {
      ptr = new WCHAR[len + CMD_LEN];
      wmemcpy(ptr, CMD, CMD_LEN);
    }
    WCHAR* path() { return ptr + CMD_LEN; }
  } command;

  if (xsdb) {
    auto x = MultiByteToWideChar(CP_ACP, 0, xsdb, -1, nullptr, 0);
    command.init(x);
    MultiByteToWideChar(CP_ACP, 0, xsdb, -1, command.path(), x);
  } else {
    auto len = SearchPathW(nullptr, XSDB_BAT, nullptr, 0, nullptr, nullptr);
    if (len == 0) {
      command.init(MAX_PATH + 1);
      if (!findXSDB(command.path())) {
        throw XsdbError("xsdb.bat not found.");
      }
    } else {
      command.init(len);
      SearchPathW(nullptr, XSDB_BAT, nullptr, len, command.path(), nullptr);
    }
  }


  // create pipes
  Pipe out; // stdout pipe
  if (!out.init(false)) throw XsdbError("Fail to create pipe.");
  Pipe err; // stderr pipe
  if (!err.init(false)) throw XsdbError("Fail to create pipe.");
  Pipe in; // stdin pipe
  if (!in.init(true)) throw XsdbError("Fail to create pipe.");

  STARTUPINFOW si;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  // set pipe
  si.dwFlags |= STARTF_USESTDHANDLES;
  si.hStdError = err.getWritePipe();
  si.hStdOutput = out.getWritePipe();
  si.hStdInput = in.getReadPipe();

  ZeroMemory(&_pi, sizeof(_pi));

  if (!CreateProcessW(
    nullptr,// lpApplicationName,
    command.ptr, // lpCommandLine,
    nullptr,// lpProcessAttributes,
    nullptr,// lpThreadAttributes,
    TRUE,   // bInheritHandles,
    0,      // dwCreationFlags,
    nullptr,// lpEnvironment,
    nullptr,// lpCurrentDirectory,
    &si,    // lpStartupInfo,
    &_pi)) { // lpProcessInformation
    std::stringstream ss;
    ss << "Fail to run 'xsdb' process";
    throw XsdbError(ss.str());
  }
  _stdin = std::move(in);
  _stdout = std::move(out);
  _stderr = std::move(err);

  Sleep(500);

# else

  if (xsdb == nullptr) {
    if (system("which xsdb > /dev/null 2>&1")) {
      // xsdb not found.
      if (std::filesystem::exists("/tools/Xilinx/Vivado/2022.1/bin/xsdb"))
        xsdb = "/tools/Xilinx/Vivado/2022.1/bin/xsdb";
      else if (std::filesystem::exists("/opt/Xilinx/Vivado/2022.1/bin/xsdb"))
        xsdb = "/opt/Xilinx/Vivado/2022.1/bin/xsdb";
    } else {
      xsdb = "xsdb";
    }
  }

  // create pipes
  Pipe out; // stdout pipe
  if (!out.init()) throw XsdbError("Fail to create pipe.");
  Pipe err; // stderr pipe
  if (!err.init()) throw XsdbError("Fail to create pipe.");
  Pipe in; // stdin pipe
  if (!in.init()) throw XsdbError("Fail to create pipe.");

  auto f = fork();
  if (f == -1) {
    throw XsdbError(
      "Fail to create 'xsdb' sub-process. Please check 'xsdb' path.");
  } else if (f == 0) {
    // Child process
    in.initAfterFork(0, false);
    out.initAfterFork(1, false);
    err.initAfterFork(2, false);

    char path[PATH_SIZE];
    strcpy(path, xsdb != nullptr ? xsdb : "xsdb");
    char* const argv[] = { path, nullptr };
    execvp(path, argv);
    exit(EXIT_FAILURE);
  }
  // parent process
  _pid = f;
  in.initAfterFork(0, true);
  out.initAfterFork(1, true);
  err.initAfterFork(2, true);
  _stdin = std::move(in);
  _stdout = std::move(out);
  _stderr = std::move(err);
  usleep(1000);
  if (!ok())
    throw XsdbError("Fail to create 'xsdb' sub-process");
# endif

  sendCommand("puts BEGIN;\r\n");
  std::string line;
  const auto start = ch::system_clock::now();
  for (;;) {
    if (_isTimeout(start, START_XSDB_TIMEOUT)) {
      throw XsdbError("xsdb process start timeout");
    }
    if (_stdout.readLine(_buffer, _bufsize, BUFFER_SIZE, START_XSDB_TIMEOUT, line)) {
      if (line == "BEGIN")
        break;
    }
  }
}

Xsdb::~Xsdb() { close(); }

bool Xsdb::ok() const
{
  if (_closed)
    return false;
#ifdef _WIN32
  switch (WaitForSingleObject(_pi.hProcess, 0)) {
  case WAIT_OBJECT_0: return false;
  }
  return true;
#else
  int status = 0;
  auto x = ::waitpid(_pid, &status, WNOHANG);
  return x == 0 ||
    !(WIFEXITED(status) || WIFSIGNALED(status) || WIFSTOPPED(status));
#endif

}

void Xsdb::close()
{
  if (_closed) return;
  _closed = true;

  if (ok()) {
    sendCommand("exit;\r\n");
#ifdef _WIN32
    WaitForSingleObject(_pi.hProcess, INFINITE);
#endif
  }

  _stdin.close();
  _stdout.close();
  _stderr.close();
#ifdef _WIN32
  CloseHandle(_pi.hProcess);
  CloseHandle(_pi.hThread);
#else
  if (ok()) {
    ::kill(_pid, SIGTERM);
    for (auto retry = 0;; retry++) {
      int status;
      if (::waitpid(_pid, &status, WNOHANG) == _pid)
        break;
      if (retry == 5)
        ::kill(_pid, SIGKILL);
    }
  }
#endif
  }

void Xsdb::sendCommand(const std::string& cmd)
{
  if (!ok())
    throw XsdbError("xsdb process exit.");
  _stdin.write(cmd);
}

void Xsdb::sendCommand(const char* cmd_str)
{
  return sendCommand(cmd_str, strlen(cmd_str));
}

void Xsdb::sendCommand(const char* cmd, size_t len)
{
  if (!ok())
    throw XsdbError("xsdb process exit.");
  _stdin.write(cmd, len);
}

template<typename... ARGS>
static void sendCommandF(Xsdb* xsdb, const char* fmt, ARGS... args)
{
  constexpr size_t BUFSIZE = 1024;
  char buffer[BUFSIZE + 2];
  auto ret = snprintf(buffer, BUFSIZE, fmt, args...);
  if (ret < 0 || ret > BUFSIZ) {
    std::stringstream ss;
    ss << "Fatal Error!! sendCommandF text format error.: " << fmt;
    throw XsdbError(ss.str());
  }

  if (ret <= 2 || (buffer[ret - 2] != '\r' && buffer[ret - 1] != '\n')) {
    ret += 2;
    buffer[ret - 2] = '\r';
    buffer[ret - 1] = '\n';
    buffer[ret] = 0;
  }

  xsdb->sendCommand(buffer, ret);
}

bool Xsdb::readLine(std::string& buffer)
{
  int x = _stdout.readLine(_buffer, _bufsize, BUFFER_SIZE, _timeout, buffer);
  if (x == -1)
    throw XsdbError("xsdb stdout read timeout");
  return x == 1;
}

bool Xsdb::readErrorLine(std::string& buffer)
{
  int x = _stderr.readLine(_ebuffer, _ebufsize, BUFFER_SIZE, _timeout, buffer);
  if (x == -1)
    throw XsdbError("xsdb stderr read timeout");
  return x == 1;
}

static void _waitCommand(Xsdb* x, const std::string& ok, uint64_t timeout,
  std::function<void(std::stringstream&)> errmsg)
{
  std::string buffer;
  const auto start = ch::system_clock::now();
  for (;;) {
    if (_isTimeout(start, timeout)) {
      std::stringstream ss;
      ss << "xsdb command timeout: ";
      errmsg(ss);
      throw XsdbError(ss.str(), true);
    }
    if (x->readLine(buffer) && buffer == ok) break;
    if (x->readErrorLine(buffer)) {
      std::stringstream ss;
      ss << "xsdb command error: ";
      errmsg(ss);
      ss << " : " << buffer;
      throw XsdbError(ss.str());
    }
  }
}


static void _waitCommand(Xsdb* x, const std::string& ok, uint64_t timeout,
  const char* errmsg)
{
  _waitCommand(x, ok, timeout, [=](std::stringstream& ss) { ss << errmsg; });
}


void Xsdb::connect()
{
  if (_connected)
    return;
  sendCommand("connect;puts COK;\r\n");
  _waitCommand(this, "COK", _timeout, "fail to connect");
  _connected = true;
}

void Xsdb::connect(const std::string& url)
{
  if (_connected)
    return;
  if (url == "")
    return connect();

  sendCommandF(this, "connect -url %s; put C2OK;", url.c_str());
  _waitCommand(this, "C2OK", _timeout, [=](std::stringstream& ss) {
    ss << "fail to connect(url: " << url << ")";
    });
  _connected = true;
}

void Xsdb::connect(const std::string& url, uint16_t port)
{
  if (_connected)
    return;
  if (url == "")
    return connect();
  if (port == 0)
    return connect(url);

  sendCommandF(this, "connect -url %s -port %u; put C2OK;", url.c_str(), port);
  _waitCommand(this, "C3OK", _timeout, [=](std::stringstream& ss) {
    ss << "fail to connect(url: " << url << ", port: " << port << ")";
    });
  _connected = true;
}

Targets Xsdb::targets()
{
  if (!_connected) throw XsdbError("Not connected.");

  sendCommand("puts [targets];puts TARGETS_DONE;\r\n");
  Targets t;
  std::string buffer;
  const auto start = ch::system_clock::now();
  for (;;) {
    if (_isTimeout(start, _timeout))
      throw XsdbError("targets command timeout.");

    if (readLine(buffer)) {
      if (buffer == "TARGETS_DONE") {
        break;
      }
      if (buffer.length() == 0) {
        continue;
      }
      t.add(buffer);
    }
    if (readErrorLine(buffer)) {
      std::stringstream ss;
      ss << "targets command error: " << buffer;
      throw XsdbError(ss.str());
    }
  }
  return t;
}


void Xsdb::target(uint32_t jtag)
{
  sendCommandF(this, "target %u; puts TOK;", jtag);
  _waitCommand(this, "TOK", _timeout, [=](std::stringstream& ss) {
    ss << "fail to set target " << jtag;
    });
  _target = jtag;
}

void Xsdb::writeRegister(uint64_t address, uint32_t value)
{
  sendCommandF(this, "mwr 0x%x 0x%x; puts WOK", address, value);
  _waitCommand(this, "WOK", _timeout, [=](std::stringstream& ss) {
    ss << "fail to write register : adress 0x" << std::hex << address << ", value 0x" << value;
    });
  if (_log) {
    printf("[Write OK ] address: 0x%08llx, value: 0x%08x (%10u)\n", address,
      value, value);
  }
}

uint32_t Xsdb::readRegister(uint64_t address)
{
  sendCommandF(this, "puts [mrd 0x%x];puts ROK", address);
  bool hasdata = false;
  std::string line;
  std::string data;
  const auto start = ch::system_clock::now();

  for (;;) {
    if (_isTimeout(start, _timeout)) {
      std::stringstream ss;
      ss << "xsdb command timeout: fail to read address 0x" << std::hex
        << address;
      throw XsdbError(ss.str(), true);
    }

    if (!readLine(line))
      continue;

    if (line == "ROK")
      break;
    if (!hasdata && !line.empty()) {
      hasdata = true;
      data = std::move(line);
    }
    if (readErrorLine(line)) {
      std::stringstream ss;
      ss << "xsdb command error2: fail to read address 0x" << std::hex << address
        << ": `" << line << "`";
      throw XsdbError(ss.str());
    }
  }

  auto pos = data.find(':');
  if (pos != std::string::npos) {
    auto left = data.find_first_not_of(" ", pos + 1);
    auto num = data.substr(left);
    size_t s = 0;
    auto v = (uint32_t)std::stoll(num, &s, 16);
    if (s != num.length()) {
      std::stringstream ss;
      ss << "xsdb command error1: fail to read address 0x" << std::hex << address
        << ", fail to parse value: " << num;
      throw XsdbError(ss.str());
    }

    if (_log) {
      printf("[Read  OK ] address: 0x%08llx, value: 0x%08x (%10u)\n", address, v,
        v);
    }

    return v;
  } else {
    std::stringstream ss;
    ss.clear(std::stringstream::goodbit);
    ss << "xsdb command error3: fail to read address 0x" << std::hex << address
      << ": " << data;
    throw XsdbError(ss.str());
  }
}


void Xsdb::writeBitstream(const std::filesystem::path& path)
{
  using namespace std::filesystem;
  if (!exists(path) || is_directory(path)) {
    std::stringstream ss;
    ss << "Bitstream not found: " << path;
    throw XsdbError(ss.str());
  }
  const auto str = path.generic_string();
  sendCommandF(this, "fpga %s;puts FOK", str.c_str());
  _waitCommand(this, "FOK", _timeout, [=](std::stringstream& ss) {
    ss << "Fail to write bitstream: " << path;
    });
  if (_log)
    printf("[Write Bin] %ls\n", path.c_str());
}

} // namespace xsdb

std::ostream& operator <<(std::ostream& o, const xsdb::TargetDevice& d)
{
  const auto indent = d.indent / 3;
  for (size_t i = 0; i < indent; i++)
    o << "  ";

  o << d.jtag << " " << d.name;
  for (size_t i = 0; i < d.lenght(); i++)
    o << std::endl << d[i];
  return o;
}

std::ostream& operator <<(std::ostream& o, const xsdb::Targets& d)
{
  for (size_t i = 0; i < d.length(); i++)
    o << d[i] << std::endl;
  return o;
}

