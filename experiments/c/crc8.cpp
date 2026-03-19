#include <iostream>
#include <cstdint>
//
// SMBus CRC8 : x^8+x^2+x+1
//            : 0b1_0000_0111 = 0x107

uint8_t crc8ApplJava(uint8_t x, uint8_t crc) {
  for (int i = 7; i >= 0; i--) {
    bool div = ((crc >> 7) ^ (x >> i)) & 0x1;
    crc = crc << 1;
    if (div) crc ^= 7;
  }
  return crc;
}

template<int N> uint8_t calcCRC8(const uint8_t(&x)[N]) {
  uint64_t crc = 0;
  for (auto v : x) {
    crc = crc8ApplJava(v, crc);
  }
  return crc;
}

void getTable(uint8_t(&tbl)[256])
{
  const uint8_t poly = 0x07;
  for (int i = 0; i < 256;i++) {
    uint8_t c = (uint8_t)i;
    for (int b = 0; b < 8; b++) {
      bool div = c & 0x80;
      c = c << 1;
      if (div) c = c ^ poly;
    }
    tbl[i] = c;
  }
}

template<int N>
uint8_t calcWithTbl(
  const uint8_t(&tbl)[256],
  const uint8_t(&data)[N])
{
  uint8_t c = 0;
  for (int i = 0; i < N; i++) {
    uint8_t idx = c ^ data[i];
    c = tbl[idx];
  }
  return c;
}

int main(int argc, char const* argv[])
{
  uint8_t addr = 0x68;
  uint8_t dw = (addr << 1) | 0;
  uint8_t dr = (addr << 1) | 1;
  uint8_t v[4] = { dw, 0x7A, dr, 0 }; // IC return 0x82 PEC code.
  const auto x = calcCRC8(v);
  if (x != 0x82) {
    printf("Error: x = %02x\n", x);
    return 1;
  }
  uint8_t tbl[256];
  getTable(tbl);
  const auto y = calcWithTbl(tbl, v);
  if (x != 0x82) {
    printf("Error: y = %02x\n", x);
    return 1;
  }
  // test
  for (unsigned int i = 0; i < 256; i++) {
    for (unsigned int i2 = 0; i2 < 256; i2++) {
      if (calcCRC8({ (uint8_t)i, (uint8_t)i2 }) != calcWithTbl(tbl, { (uint8_t)i, (uint8_t)i2 })) {
        return 1;
      }
    }
  }

  // --------------- Verilog --------------------------------
  printf("  assign crc_output = \n");
  for (int i = 0;i < 255;i++) {
    printf("      (crc_index == 8'h%02X)? 8'h%02X :\n", i, tbl[i]);
  }
  printf("                            8'h%02X;\n", tbl[255]);
  // -------------------------------------------------------

  return 0;
}

