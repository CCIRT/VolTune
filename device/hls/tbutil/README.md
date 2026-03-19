# HLS Test Bench Utility

This directory provides a GoogleTest-like testing library for HLS.

It is used to simplify unit-style testing of HLS modules and helper wrappers in the VolTune repository.

## Sample code

```cpp
#include <tbutil>

// top module XXX wrapper function to avoid cosim bug.
// This function must be defined in global scope.
static void callXXX(
  /* your HLS top module XXX function arguments*/
  hls::stream<ap_uint<32>>& a,
  hls::stream<ap_uint<32>>& b
) {
  XXX(a, b);
}

int main(int argc, char const *argv[])
{
  return RunTest(argc, argv);
}

TEST(test1) {
  hls::stream<ap_uint<32>> astream;
  hls::stream<ap_uint<32>> bstream;

  tbutil::addAll(astream, {
    1, 2, 3, 4
  });
  callXXX(astream, bstream);
}

// use test feature class
struct MyTest: public tbutil::TestFeature {

  hls::stream<ap_uint<32>> astream;
  hls::stream<ap_uint<32>> bstream;

  // virtual bool __before() {
  //   return true; // when return false, skip this test
  // }

  virtual void __after() { // after test
    // check stream is empty?
    ASSERT_EMPTY(astream);
    ASSERT_EMPTY(bstream);
  }

  // virtual void __fail() {} // when error occured

  // short cut method
  void run() { callXXX(astream, bstream); }
};

TEST_F(MyTest, test2) {
  tbutil::addAll(astream, {1, 2, 3});
  run();


}
```

## Related files

- [`../../README.md`](../../README.md), parent device-side overview

## Notes

- This utility is intended for HLS test bench development.
- The wrapper function is defined in global scope to avoid the cosim issue noted in the sample.
