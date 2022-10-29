#ifndef _SOUND_H_
#define _SOUND_H_

#include "SDL_MAINLOOP.h"

#define SOUNDS_SIZE 9

Sound* sounds[9];

void initSounds();
void freeSounds();
void handleSounds();

#endif