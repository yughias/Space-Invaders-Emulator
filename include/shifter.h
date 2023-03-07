#ifndef _SHIFTER_H_
#define _SHIFTER_H_

#define SHIFT_IN 0x03
#define SHIFTAMNT 0x02
#define SHIFT_DATA 0x04

extern uint16_t* SHIFT_16;

void initShifter();
void updateShifter();
void freeShifter();

#endif