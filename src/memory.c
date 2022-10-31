#include <hardware.h>

void initMemory(){
    MEMORY = malloc(sizeof(uint8_t)*MEMORY_SIZE);
    memset(MEMORY, 0, MEMORY_SIZE);
    memset(IO_R, 0, IO_R_SIZE);
    memset(IO_W, 0, IO_W_SIZE);
    memset(IO_W_prev, 0, IO_W_SIZE);
}

void freeMemory(){
    free(MEMORY);
}

void loadROM(){
    char extension = 'h';
    uint16_t offset = 0x0000;
    uint16_t bank_size = 0x800;
    for(int i = 0; i < 4; i++){
        char filename[15] = "ROM/invaders.h";
        filename[13] = extension;
        filename[14] = '\0';
        FILE* fptr = fopen(filename, "rb");
        fread(MEMORY+offset, 1, bank_size, fptr);
        fclose(fptr);
        extension--;
        offset += bank_size;
    }
}

uint16_t addrMirror(uint16_t addr){
    if(addr >= MEMORY_SIZE)
        addr = (addr & MIRROR_MASK) + 0x2000;
    return addr;
}