#ifndef _MEMORY_H_
#define _MEMORY_H_

#define IO_R_SIZE 4
#define IO_W_SIZE 7
#define MEMORY_SIZE 0x4000

#define MIRROR_MASK 0x3FFF

// I/0 SPACE
extern uint8_t IO_R[IO_R_SIZE];
extern uint8_t IO_W[IO_W_SIZE];
extern uint8_t IO_W_prev[IO_W_SIZE];

//RAM
extern uint8_t* MEMORY;

void initMemory();
void loadROM();
void freeMemory();
uint16_t addrMirror(uint16_t);

#endif