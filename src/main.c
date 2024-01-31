#include <hardware.h>
#include <SDL_MAINLOOP.h>

void updateJoystick();
void displayScreen();
void freeAll();

void setup(){
    frameRate(60);
    setTitle("SPC NVDRS");
    onExit = freeAll;
    size(224, 256);
    setScaleMode(NEAREST);
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
    updateJoystick();

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
    displayScreen();
}

void displayScreen(){
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

void updateJoystick(){
    const Uint8* keyState = SDL_GetKeyboardState(NULL);

    IO_R[1] = 0x00;
    IO_R[2] = 0x00;

    // coin
    if(keyState[SDL_SCANCODE_LSHIFT] || keyState[SDL_SCANCODE_RSHIFT])
        IO_R[1] |= 0b1;

    // start
    if(keyState[SDL_SCANCODE_2])
        IO_R[1] |= 0b10;
    if(keyState[SDL_SCANCODE_1])
        IO_R[1] |= 0b100;
    
    // player 1
    if(keyState[SDL_SCANCODE_S])
        IO_R[1] |= 0b10000;
    if(keyState[SDL_SCANCODE_A])
        IO_R[1] |= 0b100000;
    if(keyState[SDL_SCANCODE_D])
        IO_R[1] |= 0b1000000;

    // player 2
    if(keyState[SDL_SCANCODE_K])
        IO_R[2] |= 0b10000;
    if(keyState[SDL_SCANCODE_J])
        IO_R[2] |= 0b100000;
    if(keyState[SDL_SCANCODE_L])
        IO_R[2] |= 0b1000000;
}

void freeAll(){
    freeMemory();
    freeSounds();
    freeShifter();
}