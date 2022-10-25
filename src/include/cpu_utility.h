#ifndef _CPU_UTILITY_H_
#define _CPU_UTILITY_H_

#include "hardware.h"

// file useful for cpu internal functions
void setParity(uint16_t);
void setZero(uint16_t);
void setSign8Bit(uint8_t);
void setSign16Bit(uint16_t);

#endif