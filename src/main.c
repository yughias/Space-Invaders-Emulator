#include <hardware.h>
#include <SDL_MAINLOOP.h>

void render();
void freeAll();
void setJoypadInput(keyboard);
void unsetJoypadInput(keyboard);

void setup(){
    frameRate(60);
    setTitle("SPC NVDRS");
    onKeyPressed = setJoypadInput;
    onKeyReleased = unsetJoypadInput;
    onExit = freeAll;
    size(224, 256);
    initCPU();
    initMemory();
    initSounds();
    initShifter();
    loadROM();
    #ifdef DEBUG
        freopen("log.txt", "w", stderr);
    #endif
       
}

void loop(){
    bool midInterrupt = false;
    for(cycles = 0; cycles < 2 * 1e6 / 60;){
        memcpy(IO_W_prev, IO_W, IO_W_SIZE);
        stepCPU();
        updateShifter();
        if(midInterrupt == false && cycles >= (1e6 / 60)){
            generateInterrupt(0x08);
            midInterrupt = true;
        }
        handleSounds();
    }
    generateInterrupt(0x10);
    render();
}

void render(){
    for(int y = 0; y < 224; y++){
        for(int byte = 0; byte < 32; byte++){
            for(int i = 0; i < 8; i++){
                int offset = (y)+(height-1-(byte*8+i))*width;
                uint8_t val = MEMORY[0x2400+byte+y*32] & (1 << i);
                if(val != 0)
                    val = 255;
                else
                    val = 0;
                pixels[offset] = color(val, val, val);
            }
        }
    }
}

void setJoypadInput(keyboard keyPressed){
    if(keyPressed == '1')
        IO_R[1] |= 0b1;
    if(keyPressed == '2')
        IO_R[1] |= 0b10;
    if(keyPressed == '3')
        IO_R[1] |= 0b100;
    if(keyPressed == 's')
        IO_R[1] |= 0b10000;
    if(keyPressed == 'a')
        IO_R[1] |= 0b100000;
    if(keyPressed == 'd')
        IO_R[1] |= 0b1000000;
    if(keyPressed == 'k')
        IO_R[2] |= 0b10000;
    if(keyPressed == 'j')
        IO_R[2] |= 0b100000;
    if(keyPressed == 'l')
        IO_R[2] |= 0b1000000;
}

void unsetJoypadInput(keyboard keyReleased){
    if(keyReleased == '1')
        IO_R[1] &= ~0b00000001;
    if(keyReleased == '2')
        IO_R[1] &= ~0b00000010;
    if(keyReleased == '3')
        IO_R[1] &= ~0b00000100;
    if(keyReleased == 's')
        IO_R[1] &= ~0b00010000;
    if(keyReleased == 'a')
        IO_R[1] &= ~0b00100000;
    if(keyReleased == 'd')
        IO_R[1] &= ~0b01000000;
    if(keyReleased == 'k')
        IO_R[2] &= ~0b00010000;
    if(keyReleased == 'j')
        IO_R[2] &= ~0b00100000;
    if(keyReleased == 'l')
        IO_R[2] &= ~0b01000000;
}

void freeAll(){
    freeCPU();
    freeMemory();
    freeSounds();
    freeShifter();
}