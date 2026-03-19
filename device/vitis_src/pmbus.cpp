#include <pmbus.hpp>
#include <xparameters.h>
#include <cstdint>

#define IO ((volatile uint32_t *) (XPAR_GPIO_0_BASEADDR))
#define IOT ((volatile uint32_t *) (XPAR_GPIO_0_BASEADDR + 0x4))

namespace pmbus{

// ---------------------------------------------------
//                  IO
// ---------------------------------------------------

struct I2CIO {
  static constexpr uint8_t CLKMASK = 1; // see pmbus_io.v
  static constexpr uint8_t DATAMASK = 2;// see pmbus_io.v
  uint8_t value;
  inline I2CIO(uint8_t v): value(v) {}
  inline I2CIO(bool clk, bool data): value((clk? CLKMASK : 0) | (data? DATAMASK : 0)) {}
  inline bool getCLK() const { return value & CLKMASK;}
  inline bool isCLKLow() const { return (value & CLKMASK) == 0;}
  inline bool isCLKHigh() const { return (value & CLKMASK) != 0;}
  inline bool getDATA() const { return value & DATAMASK;}
  inline bool isDATALow() const { return (value & DATAMASK) == 0;}
  inline bool isDATAHigh() const { return (value & DATAMASK) != 0;}
};


static inline I2CIO readIO() {
  return {uint8_t(*IO >> 2)}; // see pmbus_io.v
}

static inline void setIO(bool clk, bool data) {
  uint8_t value = I2CIO(clk, data).value;
  *IOT = value | 0xFC;
  *IO = value;
}


// ---------------------------------------------------
//                  PMBus
// ---------------------------------------------------


/** State */
enum PMBusState: uint8_t {
  IDLE = 0,
  // START bit
  START, // DATA = 0
  START2,// negedge of CLK

  // Write 8bit(Data or Addr+RW)
  WBUF0, // DATA = next bit
  WBUF1, // posedge CLK
  WBUF2, // do nothing
  WBUF3, // negedge CLK

  // Data or Addr+RW ack (from device)
  SACK0, // DATA = 1
  SACK1, // posedge CLK
  SACK2, // Read Ack (Ack: DATA=0, NAK: DATA=1)
  SACK3, // negedge CLK

  // Recv 8bit data
  RDATA0, // DATA = 1
  RDATA1, // posedge CLK
  RDATA2, // Read DATA
  RDATA3, // negedge CLK

  // Recv Ack (from this program)
  MACK0, // DATA = read more data? 0 : 1
  MACK1, // posedge CLK
  MACK2, // do nothing
  MACK3, // negedge CLK

  // Restart bit
  RESTART0, // DATA = 1
  RESTART1, // posedge CLK
  RESTART2, // DATA = 0
  RESTART3, // negedge CLK

  // Stop bit
  STOP0, // DATA = 0
  STOP1, // posedge CLK
  DONE   // final state. DATA = 1
};


struct PMBus {
  uint32_t wdata;
  uint32_t rdata;
  PMBusState state;
  PMBusError error;
  uint8_t buf;
  uint8_t addr;
  uint8_t bitcount;
  uint8_t crc;
  uint8_t wsize;
  uint8_t writed;
  uint8_t rsize;
  uint8_t readed;
  bool    recv;
  bool    addrState;
  bool    busy;
};

#ifdef PMBUS_VOLATILE
volatile
#endif
PMBus gpmbus;


static void initObj()
{
  gpmbus.state = IDLE;
  gpmbus.error = SUCCESS;
  gpmbus.addr = 0;
  gpmbus.rdata = 0;
  gpmbus.wsize = 0;
  gpmbus.rsize = 0;
  gpmbus.writed = 0;
  gpmbus.readed = 0;
  gpmbus.crc = 0;
  gpmbus.busy = false;
  gpmbus.addrState = false;
  gpmbus.wdata = 0;
  gpmbus.buf = 0;
}

static void setObj(uint32_t w, uint8_t ws, uint8_t rs)
{
  gpmbus.wdata = w;
  gpmbus.rdata = 0;
  gpmbus.wsize = ws;
  gpmbus.rsize = rs;
  gpmbus.state = START;
}

/** calculate CRC*/
static uint8_t crc(bool value, uint8_t c)
{
  bool div = ((c >> 7) ^ (value? 1: 0)) & 1;
  c = c << 1;
  if (div) c ^= 7;
  return c;
}

/** update state machine.*/
void nextStep() {
  switch(gpmbus.state) {
    case IDLE:
      setIO(true, true);
      return;

    // start bit
    case START:
      if (readIO().isCLKHigh()) {
        gpmbus.state = START2;
        gpmbus.buf = gpmbus.addr << 1; // write
        gpmbus.recv = false; // send command
        gpmbus.addrState = true;
        gpmbus.bitcount = 8;
        gpmbus.error = SUCCESS;
        gpmbus.writed = gpmbus.wsize;
        auto rsize = gpmbus.rsize;
        if (rsize != 0) {
          gpmbus.readed = rsize + 1; // +1 = PEC
        } else {
          gpmbus.readed = 0;
        }
        gpmbus.crc = 0;
        setIO(true, false);
      }
      break;

    case START2:
      setIO(false, false);
      gpmbus.state = WBUF0;
      break;

    // address and r/w
    case WBUF0: // set value
    {
      auto v = (gpmbus.buf & 0x80) != 0;
      setIO(false, v);
      gpmbus.state = WBUF1;
      gpmbus.crc = crc(v, gpmbus.crc);
    }
      break;

    case WBUF1: // pos edge
      setIO(true, gpmbus.buf & 0x80);
      gpmbus.state = WBUF2;
      gpmbus.busy = false;
      break;

    case WBUF2: // do nothing

      if (readIO().isCLKHigh())
        gpmbus.state = WBUF3;
      else gpmbus.busy = true;
      break;

    case WBUF3: // neg edge
      if (gpmbus.busy) {
        gpmbus.busy = false; // delay
        return;
      }

      {
        auto buf = gpmbus.buf;
        setIO(false, buf & 0x80);
        auto bitcount = gpmbus.bitcount - 1;
        gpmbus.bitcount = bitcount;

        if (bitcount) {
          gpmbus.buf = buf << 1;
          gpmbus.state = WBUF0;
        } else {
          gpmbus.state = SACK0;
        }
      }
      break;

    // ACK from device
    case SACK0: // set value
      setIO(false, true);
      gpmbus.state = SACK1;
      break;

    case SACK1: // pos edge
      setIO(true, true);
      gpmbus.state = SACK2;
      gpmbus.busy = false;
      break;

    case SACK2: // read ack
    {
      auto read = readIO();
      if (read.isCLKHigh()) {
        if (read.isDATAHigh()) {
          gpmbus.error = gpmbus.addrState? ADDR_NOT_FOUND : gpmbus.writed == 0? PEC_ERROR : WRITE_FAIL;
        }
        gpmbus.state = SACK3;
      } else {
        gpmbus.busy = true;
      }
    }
    break;

    case SACK3: // neg edge
      if (gpmbus.busy) {
        gpmbus.busy = false; // delay
        return;
      }
      setIO(false, true);
      if (gpmbus.error) {
        gpmbus.state = STOP0;
      } else if (gpmbus.recv) {
        gpmbus.state = RDATA0;
        gpmbus.bitcount = 8;
      } else if(gpmbus.addrState) {
        gpmbus.addrState = false;
        gpmbus.state = WBUF0;
        gpmbus.buf = gpmbus.wdata;
        gpmbus.bitcount = 8;
      } else {
        auto w = gpmbus.writed;
        if (w == 0) {
          // write PEC done
          gpmbus.state = STOP0;
        } else {
          auto d = gpmbus.wdata >> 8;
          auto w1 = w - 1;
          gpmbus.writed = w1;
          gpmbus.wdata = d;

          if (w1) {
            gpmbus.state = WBUF0;
            gpmbus.buf = d;
            gpmbus.bitcount = 8;
          } else if (gpmbus.readed) {
            // RESTART
            gpmbus.state = RESTART0;
          } else {
            // write PEC & finish.
            gpmbus.state = WBUF0;
            gpmbus.buf = gpmbus.crc;
            gpmbus.bitcount = 8;
          }
        }
      }
      break;
    // -----------------------------------------------------------

    case RDATA0: // set value
      setIO(false, true);
      gpmbus.state = RDATA1;
      break;

    case RDATA1: // posedge
      setIO(true, true);
      gpmbus.state = RDATA2;
      gpmbus.busy = false;
      break;

    case RDATA2: // read value
      {
        auto read = readIO();
        // See WBUS2
        if (read.isCLKHigh()) {
          gpmbus.buf = (gpmbus.buf << 1) | (read.isDATAHigh()? 1 : 0);
          gpmbus.crc = crc(read.isDATAHigh(), gpmbus.crc);
          gpmbus.state = RDATA3;
        } else gpmbus.busy = true;

      }
      break;

    case RDATA3: // neg edge
      if (gpmbus.busy) {// delay
        gpmbus.busy = false;
        return;
      }
      setIO(false, true);
      {
        auto bitcount = gpmbus.bitcount - 1;
        gpmbus.bitcount = bitcount;

        if (bitcount) {
          gpmbus.state = RDATA0;
        } else {
          gpmbus.readed = gpmbus.readed - 1;
          gpmbus.state = MACK0;
        }
      }
      break;

    case MACK0: // set value
      setIO(false, gpmbus.readed == 0);
      gpmbus.state = MACK1;
      gpmbus.rdata = (gpmbus.rdata >> 8) | (uint32_t(gpmbus.buf) << 24);
      break;

    case MACK1: // posedge
      setIO(true, gpmbus.readed == 0);
      gpmbus.state = MACK2;
      gpmbus.busy = false;
      break;

    case MACK2: // do nothing
    {
      auto read = readIO();
      if (read.isCLKHigh())
        gpmbus.state = MACK3;
      else
        gpmbus.busy = true;
    }
    break;

    case MACK3: // neg edge
      if (gpmbus.busy) {// delay
        gpmbus.busy = false;
        return;
      }

      setIO(false, gpmbus.readed == 0);
      if (gpmbus.readed) {
        gpmbus.state = RDATA0;
        gpmbus.bitcount = 8;
      } else {
        gpmbus.state = STOP0;
        gpmbus.error = gpmbus.crc != 0 ? PEC_ERROR : SUCCESS;
      }

      break;

    // -------------------------------------------------

    case RESTART0:
      setIO(false, true);
      gpmbus.state = RESTART1;
      break;

    case RESTART1:
      setIO(true, true);
      gpmbus.state = RESTART2;
      break;

    case RESTART2:
      setIO(true, false);
      gpmbus.state = RESTART3;
      break;

    case RESTART3:
      setIO(false, false);
      gpmbus.recv = true;
      gpmbus.addrState = true;
      gpmbus.buf = (gpmbus.addr << 1) | 1;
      gpmbus.bitcount = 8;
      gpmbus.state = WBUF0;
      break;



    // -------------------------------------------------
    case STOP0:
      setIO(false, false);
      gpmbus.state = STOP1;
      break;

    case STOP1:
      setIO(true, false);
      gpmbus.state = DONE;
      break;

    case DONE:
      setIO(true, true);
      gpmbus.state = IDLE;
      break;
  }
}

static inline bool _isBusy() {return gpmbus.state != IDLE;}

bool setAddress(uint8_t addr) {
  if (_isBusy()) return false;
  gpmbus.addr = addr;
  return true;
}

bool setReadByteCmd(uint8_t cmd) {
  if (_isBusy()) return false;
  setObj(cmd, 1, 1);
  return true;
}

bool setReadWordCmd(uint8_t cmd) {
  if (_isBusy()) return false;
  setObj(cmd, 1, 2);
  return true;
}

bool setSendCmd(uint8_t cmd) {
  if (_isBusy()) return false;
  setObj(cmd, 1, 0);
  return true;
}

bool setSendByteCmd(uint8_t cmd, uint8_t data){
  if (_isBusy()) return false;
  setObj(cmd | uint32_t(data) << 8, 2, 0);
  return true;
}

bool setSendWordCmd(uint8_t cmd, uint16_t data) {
  if (_isBusy()) return false;
  setObj(cmd | uint32_t(data) << 8, 3, 0);
  return true;
}

bool isBusy() {
  return _isBusy();
}

uint8_t getByteData() {
  return (gpmbus.rdata >> 16) & 0xFF;
}

uint16_t getWordData() {
  return (gpmbus.rdata >> 8) & 0xFFFF;
}

PMBusError getError() {
  return gpmbus.error;
}

void init() {
  setIO(true, true);
  initObj();
}

}