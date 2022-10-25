#ifndef _MEMORY_H_
#define _MEMORY_H_

#define IO_R_SIZE 4
#define IO_W_SIZE 7
#define MEMORY_SIZE 0x4000

#define MIRROR_MASK 0x3FFF

// I/0 SPACE
uint8_t IO_R[4];
uint8_t IO_W[7];

//RAM
uint8_t* MEMORY;

void initMemory();
void loadROM();
void freeMemory();
uint16_t addrMirror(uint16_t);

#endif