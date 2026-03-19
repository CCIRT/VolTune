#ifndef DEVICE_HLS_COMMON_PACKBITS_HPP__
#define DEVICE_HLS_COMMON_PACKBITS_HPP__

#include <ap_int.h>
#include <limits>

namespace common {

/**
 * Utility class for handling packed binary data.
 *
 * ```
 * // Reg0 = value[0]
 * // Reg1 = value(2, 1)
 * // Reg2 = value(5, 3)
 * using bits = PackBits<1, 2, 3>;
 * using value_t = typename bits::uint; // ap_uint<6>
 * // using value_t = typename bits::sint; // ap_int<6>
 *
 * value_t value;
 * bits::template of<1>::set(value, 1); // set Reg1 = 1
 * bits::template of<2>::set(value, 3); // set Reg2 = 3
 * ap_uint<3> reg2 = bits::template of<2>::get(value);
 * ```
 */
template<int ...SIZE> struct PackBits {
private:
  template<int I, int O, int ...X> struct _bits;
  template<int I, int O, int X> struct _bits<I, O, X> {
    static constexpr int width = X;

    template<int I2> struct _of {
      static constexpr int lsb = I == I2 ? O : -1;
      static constexpr int msb = I == I2 ? O + X - 1 : -1;
      static constexpr int width = msb - lsb + 1;
      using uint = ap_uint<width>;
      using sint = ap_int<width>;
      template<typename T>
      inline static auto get(T& t) -> decltype(t(msb, lsb)) {
#pragma HLS inline
        return t(msb, lsb);
      }
      template<typename T, typename T2>
      inline static void set(T& t, const T2& t2) {
#pragma HLS inline
        t(msb, lsb) = t2;
      }
    };
  };
  template<int I, int O, int X, int ...Y> struct _bits<I, O, X, Y...> {
    using next = _bits<I + 1, O + X, Y...>;
    static constexpr int width = X + next::width;

    template<int I2> struct _of {
      using ynext = typename next::template _of<I2>;
      static constexpr int lsb = I == I2 ? O : ynext::lsb;
      static constexpr int msb = I == I2 ? O + X - 1 : ynext::msb;
      static constexpr int width = msb - lsb + 1;
      using uint = ap_uint<width>;
      using sint = ap_int<width>;
      template<typename T>
      inline static auto get(T& t) -> decltype(t(msb, lsb)) {
#pragma HLS inline
        return t(msb, lsb);
      }
      template<typename T, typename T2>
      inline static void set(T& t, const T2& t2) {
#pragma HLS inline
        t(msb, lsb) = t2;
      }
    };
  };

  using x = _bits<0, 0, SIZE...>;

public:
  static constexpr int length = sizeof...(SIZE);
  static constexpr int width = x::width;

  using uint = ap_uint<width>;
  using sint = ap_int<width>;

  template<int I> using of = typename x::template _of<I>;
  template<int I> using uint_of = typename x::template _of<I>::uint;
  template<int I> using sint_of = typename x::template _of<I>::sint;
};


}// namespace

#endif
