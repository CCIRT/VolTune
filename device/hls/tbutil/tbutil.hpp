#ifndef DEVICE_HLS_TBUTIL_TBUTIL__
#define DEVICE_HLS_TBUTIL_TBUTIL__

#include <exception>
#include <iostream>
#include <vector>
#include <sstream>
#include <numeric>
#include <type_traits>

#include <hls_stream.h>

// ---------------------------------------------------------

#define RunTest ::tbutil::Tests::main

#define TEST(name) \
  struct __##name; \
  struct __##name: public ::tbutil::TestFeature {\
    virtual void __run();\
  };\
  static ::tbutil::TestObj<__##name> __testinstance_##name(#name);\
  void __##name::__run()

#define TEST_F(className, name) \
  struct __##name; \
  struct __##name: public className {\
    virtual void __run();\
  };\
  static ::tbutil::TestObj<__##name> __testinstance_##name(#name);\
  void __##name::__run()

#define AssertEmpty(stream) \
 ::tbutil::assertStreamSize(stream, 0, #stream, __FILE__, __LINE__)

#define AssertSize(stream, size) \
 ::tbutil::assertStreamSize(stream, size, #stream, __FILE__, __LINE__)

#define AssertPop(stream, expected) \
 ::tbutil::assertStreamPop(stream, expected, __FILE__, __LINE__, #stream, #expected)

#define AssertEqual(actual, ...) \
 ::tbutil::Assert<std::remove_cv<decltype(actual)>::type>::assertEqual(actual, __VA_ARGS__, __FILE__, __LINE__, #actual, #__VA_ARGS__)

#define AssertNotEqual(actual, ...) \
 ::tbutil::Assert<std::remove_cv<decltype(actual)>::type>::assertNotEqual(actual, __VA_ARGS__, __FILE__, __LINE__, #actual, #__VA_ARGS__)

#define AssertTrue(actual) \
  if (!actual) {\
    ::tbutil::Message m(__FILE__, __LINE__); \
    m.text << #actual << " is false."; \
    throw std::runtime_error(m); \
  }

#define AssertFalse(actual) \
  if (actual) {\
    ::tbutil::Message m(__FILE__, __LINE__); \
    m.text << #actual << " is true."; \
    throw std::runtime_error(m); \
  }

// ---------------------------------------------------------

namespace tbutil {

template<typename T> struct TestContainer
{
  static std::vector<T*>& get()
  {
    static std::vector<T*> obj;
    return obj;
  }

  static void add(T* ptr) {
    get().push_back(ptr);
  }

  static int main(int argc, char const* argv[])
  {
    int result = 0;
    auto& tests = get();

    std::vector<size_t> indexes(tests.size());
    std::iota(indexes.begin(), indexes.end(), 0);

    std::vector<size_t> fails;
    std::cout << "[INFO ] Tests " << tests.size() << std::endl;

    for (auto idx : indexes) {
      auto ptr = tests[idx];
      auto r = ptr->test();
      result += r;
      if (r != 0) {
        fails.push_back(idx);
        std::cout << "[ERROR] " << ptr->name() << std::endl;
      }
      idx++;
    }
    std::cout << "======================================" << std::endl;
    if (result != 0)
      std::cout << "= Fail Test                          =" << std::endl;
    else
      std::cout << "= Success All Test                   =" << std::endl;
    std::cout << "======================================" << std::endl;
    if (result != 0) {
      std::cout << "[ERROR] Fail Test Lists " << fails.size() << std::endl;
      for (auto f : fails)
        std::cout << "    " << tests[f]->name() << std::endl;
    }
    return result;
  }

};

struct TestI {
  virtual ~TestI() {}
  virtual const char* name() const { return ""; };
  virtual int test() const { return 0; };
};

using Tests = TestContainer<TestI>;

template<typename T> struct TestObj: public TestI {
  const char* _name;
  TestObj(const char* n): _name(n) {
    Tests::add(this);
  }
  virtual ~TestObj() {}
  virtual const char* name() const { return _name; }
  virtual int test() const {
    std::cout << "[START] " << _name << std::endl;
    T t;
    int result = t.test();
    std::cout << "[END  ]";
    switch (result) {
    case -1: std::cout << " Skipped "; result = 0; break;
    case 0: std::cout << " Success "; break;
    default: std::cout << " Fail "; break;
    }
    std::cout << _name << std::endl;
    return result;
  }
};

struct TestFeature {
  /** call before __run. when return false, test will be skipped.*/
  virtual bool __before() { return true; }
  /** call after __run.*/
  virtual void __after() {}
  /** call when exception occured*/
  virtual void __fail() {}
  /** user test code.*/
  virtual void __run() {};

  int test() {
    try {
      if (!__before()) {
        return -1;
      }
      __run();
      __after();
      return 0;
    }
    catch (std::exception& e) {
      __fail();
      std::cout << "[ERROR] " << e.what() << std::endl;
      return 1;
    }
  }
};

struct Message {
  std::ostringstream text;
  Message(const char* file, int line): text()
  {
    text << "[" << file << ":" << line << "] ";
  }
  operator std::string() const { return text.str(); }


  template<typename T> void append(const T& t) { text << t; }
  void append(const uint8_t& t) { text << (uint32_t)t; }
  void append(const ap_uint<1>& t) { text << (uint32_t)t; }
  void append(const ap_uint<2>& t) { text << (uint32_t)t; }
  void append(const ap_uint<3>& t) { text << (uint32_t)t; }
  void append(const ap_uint<4>& t) { text << (uint32_t)t; }
  void append(const ap_uint<5>& t) { text << (uint32_t)t; }
  void append(const ap_uint<6>& t) { text << (uint32_t)t; }
  void append(const ap_uint<7>& t) { text << (uint32_t)t; }
  void append(const ap_uint<8>& t) { text << (uint32_t)t; }
  void append(const bool& t) {
    if (t)
      text << "true";
    else
      text << "false";
  }
};

template<typename A> struct is_hls_axis {
  static constexpr bool value = false;
};

template<typename T, std::size_t WUser, std::size_t WId, std::size_t WDest>
struct is_hls_axis<hls::axis<T, WUser, WId, WDest>> {
  static constexpr bool value = true;
};

template<typename A0, typename E0>
struct __IsSameAE {
  using A = typename std::remove_cv<A0>::type;
  using E = typename std::remove_cv<E0>::type;
  static constexpr bool isSameType = std::is_same<A, E>::value;
  static constexpr bool isAxisA = is_hls_axis<A>::value;
  static constexpr bool isAxisE = is_hls_axis<E>::value;
  static constexpr bool isAxis = isAxisA && isAxisE;
  static constexpr bool eIsScalar = std::is_scalar<E>::value;
  static constexpr bool EToA = std::is_convertible<E, A>::value;
  static constexpr bool AToE = std::is_convertible<A, E>::value;
  static constexpr int  Mode =
    isSameType ? 0 :
    isAxis ? 1 :
    isAxisA && !isAxisE ? 2 :
    eIsScalar ? 3 :
    EToA ? 4 :
    AToE ? 5 :
    6;
};

template<typename A, typename E, int N> struct __IsSameAE2 {
  static bool eq(const A& a, const E& e) {
    return a == e;
  }
};

template<
  typename T1, std::size_t WUser1, std::size_t WId1, std::size_t WDest1,
  typename T2, std::size_t WUser2, std::size_t WId2, std::size_t WDest2,
  int N> struct __IsSameAE2<
  hls::axis<T1, WUser1, WId1, WDest1>,
  hls::axis<T2, WUser2, WId2, WDest2>,
  N> {
  static bool eq(
    const hls::axis<T1, WUser1, WId1, WDest1>& a,
    const hls::axis<T2, WUser2, WId2, WDest2>& e) {
      return a.data == e.data && a.keep == e.keep && a.user == e.user && a.dest == e.dest;
  }
};


template<typename A, typename E> struct __IsSameAE2<A, E, 2> {
  static bool eq(const A& a, const E& e) {
    // E must be implements operator==(const axis&)
    return e == a;
  }
};

template<typename A, typename E> struct __IsSameAE2<A, E, 4> {
  static bool eq(const A& a, const E& e) {
    return a == (A)e;
  }
};

template<typename A, typename E> struct __IsSameAE2<A, E, 5> {
  static bool eq(const A& a, const E& e) {
    return (E)a == e;
  }
};


template<typename A, typename E>
bool isSameAE(const A& a, const E& e) {
  using T = __IsSameAE<A, E>;
  return __IsSameAE2<typename T::A, typename T::E, T::Mode>::eq(a, e);
}

template<typename T> struct Assert {
  template<typename E>
  static void assertEqual(
    const T& actual,
    const E& expected,
    const char* file, size_t line,
    const char* nameA, const char* nameE)
  {
    if (!isSameAE(actual, expected)) {
      Message m(file, line);
      m.text << nameA << " != " << nameE << std::endl << "    Actual  : ";
      m.append(actual);
      m.text << std::endl << "    Expected: ";
      m.append(expected);
      throw std::runtime_error(m);
    }
  }
  template<typename E>
  static void assertNotEqual(
    const T& actual,
    const E& expected,
    const char* file, size_t line,
    const char* nameA, const char* nameE)
  {
    if (!isSameAE(actual, expected)) {
      Message m(file, line);
      m.text << nameA << " == " << nameE << std::endl << "    Actual: ";
      m.append(actual);
      throw std::runtime_error(m);
    }
  }
};

template<typename T> struct GetElement {
  static auto get(const T& v, size_t i) -> decltype(v[i]) {
    return v[i];
  }
};

template<typename T> struct GetElement<hls::stream<T>> {
  static T get(const T& v, size_t i) {
    // ignore i
    return v.read();
  }
};

template<typename T> struct Assert<hls::stream<T>> {
  template<typename E>
  static void assertEqual(
    hls::stream<T>& actual,
    const E& expected,
    const char* file, size_t line,
    const char* nameA, const char* nameE)
  {
    auto s = actual.size();
    auto se = expected.size();
    if (s != se) {
      Message m(file, line);
      m.text << nameA << " != " << nameE << std::endl
        << "    Actual stream size  : " << s << std::endl
        << "    Expected stream size: " << se;
      for (size_t i = 0; i < s; i++) {
        m.text << std::endl
          << "      Actual[" << i << "] :";
        m.append(actual.read());
        m.text << std::endl
          << "    Expected[" << i << "] :";
        if (i < se)
          m.append(GetElement<E>::get(expected, i));
        else
          m.text << "----";
      }
      throw std::runtime_error(m);

    }
    for (size_t i = 0; i < s; i++) {
      auto a = actual.read();
      auto e = GetElement<E>::get(expected, i);
      if (!isSameAE(a, e)) {
        Message m(file, line);
        m.text << nameA << " != " << nameE << std::endl << "      Actual[" << i << "] :";
        m.append(a);
        m.text << std::endl << "    Expected[" << i << "] :";
        m.append(e);
        throw std::runtime_error(m);
      }
    }
  }

  template<typename E>
  static void assertEqual(
    hls::stream<T>& actual,
    std::initializer_list<E> expected,
    const char* file, size_t line,
    const char* nameA, const char* nameE)
  {
    auto ptr = expected.begin();
    const auto end = expected.end();
    std::vector<E> e;
    for (;ptr != end; ptr++)
      e.push_back(*ptr);
    assertEqual(actual, e, file, line, nameA, nameE);
  }



  template<typename E>
  static void assertNotEqual(
    hls::stream<T>& actual,
    const E& expected,
    const char* file, size_t line,
    const char* nameA, const char* nameE)
  {
    auto s = actual.size();
    auto se = expected.size();
    if (s != se) {
      return; //OK
    }
    for (size_t i = 0; i < s; i++) {
      auto a = actual.read();
      auto e = GetElement<E>::get(expected, i);
      if (!isSameAE(a, e)) {
        return; // OK
      }
    }

    Message m(file, line);
    m.text << nameA << " == " << nameE;
    throw std::runtime_error(m);
  }

  template<typename E>
  static void assertNotEqual(
    hls::stream<T>& actual,
    std::initializer_list<E> expected,
    const char* file, size_t line,
    const char* nameA, const char* nameE)
  {
    std::vector<E> e;
    auto ptr = expected.begin();
    auto end = expected.end();
    for (;ptr != end; ptr++) {
      e.push_back(*ptr);
    }
    assertNotEqual(actual, e, file, line, nameA, nameE);
  }

};


template<typename T>
void assertStreamSize(
  hls::stream<T>& stream,
  size_t size,
  const char* streamName,
  const char* file, int line)
{
  auto s = stream.size();
  if (s != size) {
    Message e(file, line);
    e.text << streamName << ".size() = " << s << ", expected " << size;
    for (auto i = 0;i < s; i++) {
      e.text << std::endl << "    " << streamName << "[" << i << "]: ";
      e.append(stream.read());
    }
    throw std::runtime_error(e);
  }
}

template<typename T, typename T2>
void assertStreamPop(
  hls::stream<T>& stream,
  const T2& expected,
  const char* file, int line,
  const char* streamName, const char* expectedText)
{
  if (stream.empty()) {
    Message e(file, line);
    e.text << streamName << " is empty" << std::endl
      << "  Actual: ---" << std::endl
      << "  Expect: ";
    e.append(expected);
    throw std::runtime_error(e);
  }
  auto s = stream.read();
  if (!isSameAE(s, expected)) {
    Message e(file, line);
    e.text << streamName << ".read() != " << expectedText << std::endl
      << "  Actual: " << s << std::endl
      << "  Expect: " << expected;
    throw std::runtime_error(e);
  }
}


template<typename T, typename T2>
void addAll(
  hls::stream<T>& stream,
  std::initializer_list<T2> d)
{
  auto ptr = d.begin();
  auto end = d.end();
  for (;ptr != end; ptr++)
    stream.write(*ptr);
}

template<typename T>
void clearStream(hls::stream<T>& stream)
{
  while(!stream.empty()) {
    stream.read();
  }
}

}// end namespace tbutil

// ---------------------------------------------------------
// cout

template<typename T, std::size_t WUser, std::size_t WId, std::size_t WDest>
std::ostream& operator<< (std::ostream& o, const hls::axis<T, WUser, WId, WDest>& v)
{
  const auto f = o.flags();
  o << std::hex
    << "{ TDATA: " << v.data
    << ", TKEEP: " << v.keep
    << ", TSTRB: " << v.strb
    << ", TLAST: " << v.last;
  if (WUser != 0)
    o << ", TUSER: " << v.user;
  if (WId != 0)
    o << ", TID: " << v.id;
  if (WDest != 0)
    o << ", TDEST: " << v.dest;
  o << " }";
  o.flags(f);
  return o;
}


// ---------------------------------------------------------
#endif
