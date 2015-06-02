#include <csp/csp.h>
#include <dev/i2c.h>
#include <util/error.h>
#include <util/log.h>

// turn on wheel
int rwlib_turnon(uint8_t dest_addr);
// turn off wheel
int rwlib_turnoff(uint8_t dest_addr);

// Ping
int rwlib_ping(uint8_t dest_addr);

int rwlib_receiveModeTelem(uint8_t dest_addr, uint8_t *mode_type, float *mode_value);
int rwlib_receiveParamTelem(uint8_t dest_addr, uint8_t *param_number, float *param_value);
int rwlib_sendModeCommand(uint8_t dest_addr, uint8_t mode_number, float mode_value);
int rwlib_sendParamCommand(uint8_t dest_addr, uint8_t param_number, float param_value);

// short form functional test
// housekeeping? Print args

// send speed command
// send torque command

