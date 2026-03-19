#ifndef SW_PMBUS_HPP__
#define SW_PMBUS_HPP__

#include <cstdint>

/**
 * User implementation method.
 * @see pmbus::isBusy()
 * @see pmbus::nextStep()
 */
void runPMBus();


enum PMBusError: uint8_t {
  SUCCESS = 0,
  ADDR_NOT_FOUND,
  WRITE_FAIL,
  PEC_ERROR
};


namespace pmbus{

/** init PMBus */
void init();

/** set target device address. */
bool setAddress(uint8_t addr);

/** Is PMBus state busy?*/
bool isBusy();

/**
 * Update PMBus state machine.
 * Repeated calls to this function progress the sending and receiving process.
 * Before calling this function, set the device address with the `setAddress` function
 * and register the command you wish to execute with `setReadByteCmd`, `setReadWordCmd`, `setSendCmd`, `setSendByteCmd` or `setSendWordCmd`.
 * This function should be executed repeatedly until the `isBusy` function returns false.
 * This function should be called at a frequency 4 times higher than the desired transfer rate.
 */
void nextStep();


/** register read byte command. @see readByteData() @see nextStep()*/
bool setReadByteCmd(uint8_t cmd);

/** register read word command. @see readWordData() @see nextStep()*/
bool setReadWordCmd(uint8_t cmd);

/** register send command. @see nextStep()*/
bool setSendCmd(uint8_t cmd);

/** register send byte data command. @see nextStep()*/
bool setSendByteCmd(uint8_t cmd, uint8_t data);

/** register send word data command. @see nextStep()*/
bool setSendWordCmd(uint8_t cmd, uint16_t data);

/** get byte command(readByteCmd) result. @see readByteCmd*/
uint8_t getByteData();

/** get word command(readWordCmd) result. @see readWordCmd*/
uint16_t getWordData();

/** get error status */
PMBusError getError();

// short cut
constexpr uint8_t PAGE = 0;
constexpr uint8_t VOUT_COMMAND = 0x21;
constexpr uint8_t READ_VOUT = 0x8B;
constexpr uint8_t READ_IOUT = 0x8C;
constexpr uint8_t READ_POUT = 0x96;
constexpr uint8_t VOUT_UV_WARN_LIMIT = 0x43;
constexpr uint8_t VOUT_UV_FAULT_LIMIT = 0x44;
constexpr uint8_t POWER_GOOD_ON = 0x5E;
constexpr uint8_t POWER_GOOD_OFF = 0x5F;

inline bool setPage(uint8_t page) {return setSendByteCmd(PAGE, page);}
inline bool setVout(uint16_t l16) { return setSendWordCmd(VOUT_COMMAND, l16); }
inline bool setReadVout() { return setReadWordCmd(READ_VOUT);}
inline bool setReadIout() { return setReadWordCmd(READ_IOUT);}
inline bool setReadPout() { return setReadWordCmd(READ_POUT);}
inline bool setVoutUVWarnLimit(uint16_t v) { return setSendWordCmd(VOUT_UV_WARN_LIMIT, v);}
inline bool setVoutUVFaultLimit(uint16_t v) { return setSendWordCmd(VOUT_UV_FAULT_LIMIT, v);}
inline bool setPowerGoodOn(uint16_t v) { return setSendWordCmd(POWER_GOOD_ON, v);}
inline bool setPowerGoodOff(uint16_t v) { return setSendWordCmd(POWER_GOOD_OFF, v);}


}

#endif
