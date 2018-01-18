#include <csp/csp.h>
#include <dev/i2c.h>
#include <util/error.h>
#include <util/log.h>

#include "common_types.h"

#define RW_MODE_IDLE     0x00
#define RW_MODE_CURRENT  0x02
#define RW_MODE_SPEED    0x05
#define RW_MODE_TORQUE   0x16
#define RW_MODE_BURNIN   0x17
#define RW_MODE_SFFT     0x18

#define RW_PARAM_TEMP        0x03
#define RW_PARAM_SPEED       0x05
#define RW_PARAM_TORQUE      0x1C
#define RW_PARAM_SEU_COUNT   0x29
#define RW_PARAM_SFFT_START  0x80
#define RW_PARAM_SFFT_END    0xE3

// turn on wheel
int rwlib_turnon(uint8 dest_addr);
// turn off wheel
int rwlib_turnoff(uint8 dest_addr);

// Ping
int rwlib_ping(uint8 dest_addr);

int rwlib_receiveModeTelem(uint8 dest_addr, uint8 *mode_type, float *mode_value);
int rwlib_receiveParamTelem(uint8 dest_addr, uint8 *param_number, float *param_value);
int rwlib_sendModeCommand(uint8 dest_addr, uint8 mode_number, float mode_value);
int rwlib_sendParamCommand(uint8 dest_addr, uint8 param_number, float param_value);

// short form functional test
// housekeeping? Print args

// send speed command
// send torque command
