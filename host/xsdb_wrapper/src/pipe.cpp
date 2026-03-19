#include "pipe.hpp"
#include "xsdb_error.hpp"
#include <chrono>
#include <sstream>
#include <string.h>

#ifndef _WIN32
#include <fcntl.h>
#include <unistd.h>
#endif

namespace xsdb {

enum class OSResult {
  CONTINUE,
  SUCCESS,
  NODATA,
  RETURN_LAST
};

#ifdef _WIN32

// -----------------------------------------------------
//                     Windows
// -----------------------------------------------------
#define INVALID_PIPE INVALID_HANDLE_VALUE

bool Pipe::init(bool stdIn) {
  // Generate Named Pipe
  static volatile LONG64 ID = 1;
  const auto id = InterlockedIncrement64(&ID);
  const auto pid = GetCurrentProcessId();
  char lpName[256];
  sprintf(lpName, "\\\\.\\pipe\\XSDB.ANO.PIPE%08lu.%08llu", pid, id);
  SECURITY_ATTRIBUTES saAttr;
  ZeroMemory(&saAttr, sizeof(saAttr));
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;

  _r = CreateNamedPipeA(lpName,
    PIPE_ACCESS_INBOUND,
    PIPE_TYPE_BYTE | PIPE_NOWAIT,
    1,
    2048,
    2048,
    1000,
    &saAttr);
  if (_r == INVALID_HANDLE_VALUE) return false;

  _w = CreateFileA(lpName,
    GENERIC_WRITE,
    0,
    &saAttr,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    nullptr);
  if (_w == INVALID_HANDLE_VALUE) {
    CloseHandle(_r);
    _r = INVALID_HANDLE_VALUE;
    return false;
  }

  SetHandleInformation(stdIn ? _w : _r, HANDLE_FLAG_INHERIT, 0);
  return true;
}

void Pipe::close()
{
  if (_r != INVALID_HANDLE_VALUE) {
    CloseHandle(_r);
    _r = INVALID_HANDLE_VALUE;
  }
  if (_w != INVALID_HANDLE_VALUE) {
    CloseHandle(_w);
    _w = INVALID_HANDLE_VALUE;
  }
}


static size_t _writePipeOs(HANDLE _w, const char* text, size_t length)
{
  DWORD dwWritten;
  if (!WriteFile(_w, text, length, &dwWritten, NULL)) {
    throw XsdbError("Fail to write stdin of 'xsdb'");
  }
  return (size_t)dwWritten;
}

static OSResult _readPipeOs(HANDLE _r, size_t& bufsize, char* buffer, size_t bufferLength, const std::string& outputBuffer)
{
  DWORD dwRead;

  if (!ReadFile(_r, buffer, bufferLength, &dwRead, NULL)) {
    const auto err = GetLastError();
    switch (err) {
    case ERROR_NO_DATA:
      return outputBuffer.length() == 0 ? OSResult::NODATA : OSResult::CONTINUE;

    case ERROR_BROKEN_PIPE:
      return outputBuffer.length() != 0 ? OSResult::NODATA : OSResult::RETURN_LAST;

    default:
      // throw error
      bufsize = 0;
      std::stringstream ss;
      ss << "Fail to read pipe of 'xsdb': Error code(" << err << ")";

      char lpBuffer[1024];
      const auto len = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK, NULL, err, 0, lpBuffer, sizeof(lpBuffer), NULL);
      if (len == 0)
        SetLastError(err);
      else
        ss << ", " << lpBuffer;
      throw XsdbError(ss.str());
    }
  }
  bufsize = dwRead;
  return OSResult::SUCCESS;
}

#else

// -----------------------------------------------------
//                     Linux
// -----------------------------------------------------
#define INVALID_PIPE 0
bool Pipe::init()
{
  int pipes[2];
  constexpr int READ = 0;
  constexpr int WRITE = 1;

  if (::pipe2(pipes, O_NONBLOCK) == EXIT_SUCCESS) {
    _r = pipes[READ];
    _w = pipes[WRITE];
    return true;
  }
  return false;
}
void Pipe::close()
{
  if (_r) {
    ::close(_r);
    _r = 0;
  }
  if (_w) {
    ::close(_w);
    _w = 0;
  }
}

void Pipe::initAfterFork(int stdmode, bool hostProcess)
{
  if (hostProcess) {
    ::close(stdmode == 0? _r : _w);
  } else {
    if (stdmode == 0) {
      ::close(_w);
      ::dup2(_r, STDIN_FILENO);
      ::close(_r);
    } if (stdmode == 1) {
      ::close(_r);
      ::dup2(_w, STDOUT_FILENO);
      ::close(_w);
    } else {
      ::close(_r);
      ::dup2(_w, STDERR_FILENO);
      ::close(_w);
    }
  }
}

static size_t _writePipeOs(int fd, const char* text, size_t length)
{
  const auto ret = ::write(fd, text, length);
  if (ret == -1) {
    std::stringstream ss;
    ss << "xsdb pipe write error: errno: " << errno;
    throw XsdbError(ss.str());
  }
  return (size_t)ret;
}

static OSResult _readPipeOs(int fd, size_t& bufsize, char* buffer, size_t bufferLength, const std::string& outputBuffer)
{
  auto ret = ::read(fd, buffer, bufferLength);
  if (ret == -1) {
    if (errno == EAGAIN) {
      return outputBuffer.length() == 0 ? OSResult::NODATA : OSResult::CONTINUE;
    }
    throw XsdbError("Fail to read pipe of 'xsdb'");
  }

  bufsize = (size_t)ret;
  return OSResult::SUCCESS;
}

#endif

// -----------------------------------------------------
//                  Windows / Linux
// -----------------------------------------------------

Pipe::Pipe(): _r(INVALID_PIPE), _w(INVALID_PIPE){}
Pipe::Pipe(Pipe&& p): _r(p._r), _w(p._w)
{
  p._r = p._w = INVALID_PIPE;
}

Pipe& Pipe::operator=(Pipe&& p)
{
  _r = p._r;
  _w = p._w;
  p._r = p._w = INVALID_PIPE;
  return *this;
}


void Pipe::write(const char* text, size_t length) const
{
  if (!text || !length) return;
  size_t w = 0;
  while (w != length)
    w += _writePipeOs(_w, text + w, length - w);
}


int Pipe::readLine(
  char* buffer,
  size_t& bufferSize,
  size_t bufferLength,
  uint64_t timeout,
  std::string& outputBuffer) const
{
  if (!buffer) throw new std::invalid_argument("buffer is nullptr");
  if (!bufferLength) throw new std::invalid_argument("buffer length is 0");
  outputBuffer.clear();

  const auto bs = bufferSize;
  for (size_t i = 0; i < bs; i++) {
    if (buffer[i] == '\n') {
      const auto len = (i != 0 && buffer[i - 1] == '\r') ? i - 1 : i;
      outputBuffer += std::string_view(buffer, len);
      if (i + 1 < bs) {
        ::memcpy(buffer, buffer + i + 1, bs - i - 1);
        bufferSize = bs - (i + 1);
      } else {
        bufferSize = 0;
      }
      return true;
    }
  }

  size_t len = 0;
  using sclk = std::chrono::system_clock;
  const auto start = sclk::now();
  size_t bufsize = bufferSize;
  for (;;) {
    if (timeout != 0) {
      const auto time = sclk::now() - start;
      const auto mcount = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
      if (mcount < 0)
        return false;
      if (timeout < (size_t)mcount) return -1;
    }

    if (bufsize != 0) {
      len += bufsize;
      outputBuffer += std::string_view(buffer, bufsize);
    }
    bufsize = 0;

    switch (_readPipeOs(_r, bufsize, buffer, bufferLength, outputBuffer)) {
    case OSResult::CONTINUE: continue;
    case OSResult::NODATA: return 0;
    case OSResult::RETURN_LAST: return 1;
    case OSResult::SUCCESS: break;
    }

    for (size_t i = 0; i < bufsize; i++) {
      if (buffer[i] == '\n') {
        const auto len = (i != 0 && buffer[i - 1] == '\r') ? i - 1 : i;
        outputBuffer += std::string_view(buffer, len);
        if (i + 1 < bufsize) {
          ::memcpy(buffer, buffer + i + 1, bufsize - i - 1);
          bufferSize = bufsize - (i + 1);
        } else {
          bufferSize = 0;
        }
        return 1;
      }
    }
  }
}

} // end namespace
