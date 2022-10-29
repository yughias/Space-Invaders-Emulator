#include <cpu_utility.h>

void setParity(uint16_t val){
    int counter = 0;
    while(val != 0){
        counter += (val & 0x1);
        val = val >> 1;
    }
    bool parity = (counter % 2 == 0);
    if(parity)
        *F_8 |= SET_P;
    else
        *F_8 &= CLEAR_P;
}

void setZero(uint16_t val){
    if(val == 0)
        *F_8 |= SET_Z;
    else
        *F_8 &= CLEAR_Z;
}

void setSign8Bit(uint8_t val){
    if(val & 0x80)
        *F_8 |= SET_S;
    else
        *F_8 &= CLEAR_S;
}

void setSign16Bit(uint16_t val){
    if(val & 0x8000)
        *F_8 |= SET_S;
    else
        *F_8 &= CLEAR_S;
}