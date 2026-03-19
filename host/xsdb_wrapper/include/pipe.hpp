#ifndef PIPE_HPP__
#define PIPE_HPP__

#include <string>
#include <string_view>

#ifdef _WIN32
# include <windows.h>
#endif


namespace xsdb {

class Pipe {
public:
#ifdef _WIN32
  using pipe_t = HANDLE;
#else
  using pipe_t = int;
#endif

  Pipe();
  Pipe(Pipe&& p);
  Pipe& operator=(Pipe&& p);

  ~Pipe() { close(); }
  void close();
#ifdef _WIN32
  bool init(bool stdIn);
#else
  bool init();
  /**
   * @param stdmode 0: stdin, 1: stdout, 2: stderr
   */
  void initAfterFork(int stdmode, bool hostProcess);
#endif
  /**
   * @brief write text to wirte pipe
   *
   * @param text string
   */
  void write(const char* text, size_t length) const;
  void write(const std::string& str) const { write(str.c_str(), str.length());}

  /**
   * @brief read line from read pipe
   *
   * @param buffer buffer(not nullptr)
   * @param bufferSize `buffer` valid data size
   * @param bufferLength `buffer` array size
   * @param timeout
   * @param outputBuffer output buffer
   * @return 1: success, 0: no data, -1: timeout
   */
  int readLine(
    char* buffer,
    size_t& bufferSize,
    size_t bufferLength,
    uint64_t timeout,
    std::string& outputBuffer) const;

  pipe_t getWritePipe() const { return _w; }
  pipe_t getReadPipe() const { return _r; }


private:
  Pipe(const Pipe&) = delete;
  pipe_t _r;
  pipe_t _w;
};

}
#endif
