#include <hardware.h>

void initShifter(){
    SHIFT_16 = malloc(sizeof(uint16_t));
    *SHIFT_16 = 0;
}

void updateShifter(){
    uint16_t val = IO_W[SHIFT_DATA];
    *SHIFT_16 = (val << 8) | ((*SHIFT_16) >> 8);
    IO_R[SHIFT_IN] = ((*SHIFT_16) >> (8-IO_W[SHIFTAMNT]) & 0xFF);
}

void freeShifter(){
    free(SHIFT_16);   
}