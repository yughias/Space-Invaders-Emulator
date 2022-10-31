#ifndef _SDL_MAINLOOP_H_
#define _SDL_MAINLOOP_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"

#define frameRate(x) frameRate = x

#define LEFT SDLK_LEFT
#define RIGHT SDLK_RIGHT
#define UP SDLK_UP
#define DOWN SDLK_DOWN
#define BUTTON_LEFT SDL_BUTTON_LEFT
#define BUTTON_RIGHT SDL_BUTTON_RIGHT

typedef int keyboard;
typedef Uint32 button;

typedef Mix_Chunk Sound;

extern unsigned int displayWidth;
extern unsigned int displayHeight;
extern unsigned int width;
extern unsigned int height;
extern int* pixels;

extern unsigned int frameRate;
extern unsigned int frameCount;
extern float deltaTime;

extern int pmouseX;
extern int pmouseY;
extern int mouseX;
extern int mouseY;
extern bool isMousePressed;
extern bool isMouseDragged;
extern button mouseButton;
extern bool isKeyPressed;
extern bool isKeyReleased;
extern keyboard keyPressed;
extern keyboard keyReleased;

extern void setup();
extern void loop();
extern void size(int, int);
extern void setTitle(const char*);
extern Uint64 millis();
extern void fullScreen();
extern void background(int, int, int);
extern int color(int, int, int);
extern int red(int);
extern int green(int);
extern int blue(int);

// Audio function 
extern Sound* loadSound(const char*);
extern void playSound(Sound*);
extern void freeSound(Sound*);

//Custom close function
extern void onClose();
extern void setJoypadInput();
extern void unsetJoypadInput();

#endif