#include <hardware.h>

Sound* sounds[SOUNDS_SIZE];

void initSounds(){
    char filename[] = "data/X.wav";
    char soundName = '0';
    for(int i = 0; i < SOUNDS_SIZE; i++){
        filename[5] = soundName;
        sounds[i] = loadSound(filename);
        soundName++;
    }
}

void freeSounds(){
    for(int i = 0; i < SOUNDS_SIZE; i++)
        freeSound(sounds[i]);
}

void handleSounds(){
    if((IO_W[3] & 0b1) && ((IO_W_prev[3] & 0b1) == 0))
        playSound(sounds[0]);
    if((IO_W[3] & 0b10) && ((IO_W_prev[3] & 0b10) == 0))
        playSound(sounds[1]);
    if((IO_W[3] & 0b100) && ((IO_W_prev[3] & 0b100) == 0))
        playSound(sounds[2]);
    if((IO_W[3] & 0b1000) && ((IO_W_prev[3] & 0b1000) == 0))
        playSound(sounds[3]);

    if((IO_W[5] & 0b1) && ((IO_W_prev[5] & 0b1) == 0))
        playSound(sounds[4]);
    if((IO_W[5] & 0b10) && ((IO_W_prev[5] & 0b10) == 0))
        playSound(sounds[5]);
    if((IO_W[5] & 0b100) && ((IO_W_prev[5] & 0b100) == 0))
        playSound(sounds[6]);
    if((IO_W[5] & 0b1000) && ((IO_W_prev[5] & 0b1000) == 0))
        playSound(sounds[7]);
    if((IO_W[5] & 0b10000) && ((IO_W_prev[5] & 0b10000) == 0))
        playSound(sounds[8]);
}