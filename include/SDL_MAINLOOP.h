#ifndef _SDL_MAINLOOP_H_
#define _SDL_MAINLOOP_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

void setup();
void loop();
void size(int, int);
void setTitle(const char*);
Uint64 millis();
void fullScreen();
void background(int, int, int);
int color(int, int, int);
int red(int);
int green(int);
int blue(int);

// Audio function 
Sound* loadSound(const char*);
void playSound(Sound*);
void freeSound(Sound*);

// Custom callbacks
extern void (*onExit)();
extern void (*onKeyPressed)(keyboard);
extern void (*onKeyReleased)(keyboard);

#endif