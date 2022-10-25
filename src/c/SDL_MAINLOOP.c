#include "../include/SDL_MAINLOOP.h"

#define MAX_NAME  64

unsigned int displayWidth;
unsigned int displayHeight;
unsigned int width;
unsigned int height;
int* pixels;

unsigned int frameRate;
unsigned int frameCount;
float deltaTime;

int pmouseX;
int pmouseY;
int mouseX;
int mouseY;
bool isMousePressed;
bool isMouseDragged;
button mouseButton;
bool isKeyPressed;
bool isKeyReleased;
keyboard keyPressed;
keyboard keyReleased;

void setup();
void loop();
void size(int, int);
void fullScreen();
void background(int, int, int);
int color(int, int, int);
int red(int);
int green(int);
int blue(int);

Sound* loadSound(const char*);
void playSound(Sound*);
void freeSound(Sound*);

SDL_Window* window;
SDL_Surface* surface;
bool running = false;
Uint32 winFlags = SDL_WINDOW_SHOWN;
char windowName[64];

void SDL_MAINLOOP_addAudioHandler(SDL_AudioDeviceID, Uint64, Uint64);
void SDL_MAINLOOP_HandleAudio();

int main(int argc, char* argv[]){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    isMousePressed = false;
    isMouseDragged = false;
    isKeyPressed = false;
    isKeyReleased = false;
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    displayWidth = displayMode.w;
    displayHeight = displayMode.h;
    width = 800;
    height = 600;
    frameRate = 60;
    frameCount = 0;
    strcpy(windowName, "window");
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_Volume(-1, 5);
    setup();

    window = SDL_CreateWindow(windowName, 100, 100, width, height, winFlags);
    surface = SDL_GetWindowSurface(window);
    pixels = (int*)surface->pixels;
    width = surface->w;
    height = surface->h;

    Uint64 a_clock = SDL_GetPerformanceCounter();
    Uint64 b_clock = SDL_GetPerformanceCounter();

    running = true;
    while(running){
        a_clock = SDL_GetPerformanceCounter();
        deltaTime = (float)(a_clock - b_clock)/SDL_GetPerformanceFrequency()*1000;

        if(deltaTime > 1000.0f / frameRate){
            frameCount++;
            isKeyReleased = false;
            pmouseX = mouseX;
            pmouseY = mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            SDL_Event event;
            isKeyReleased = false;
            isKeyPressed = false;
            while(SDL_PollEvent(&event)){
                if(event.window.event == SDL_WINDOWEVENT_CLOSE || event.key.keysym.sym == SDLK_ESCAPE)
                    running = 0;

                if(event.button.type == SDL_MOUSEBUTTONDOWN){
                    isMousePressed = true;
                    mouseButton = event.button.button;
                }

                if(event.button.type == SDL_MOUSEBUTTONUP)
                    isMousePressed = false;

                if(event.key.state == SDL_PRESSED){
                    keyPressed = event.key.keysym.sym;
                    isKeyPressed = true;
                }

                if(event.key.type == SDL_KEYUP){
                    isKeyReleased = true;
                    keyReleased = event.key.keysym.sym;
                }
            }

            if(isMousePressed && (mouseX != pmouseX || mouseY != pmouseY))
                isMouseDragged = true;
            else
                isMouseDragged = false;

            loop();
            SDL_UpdateWindowSurface(window);

            b_clock = SDL_GetPerformanceCounter();
        }

    };

    onClose();
    Mix_CloseAudio();
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void size(int w, int h){
    if(!running){
        width = w;
        height = h;
    }
}

void setTitle(const char* name){
    strncpy(windowName, name, MAX_NAME);
}

Uint64 millis(){
    return (float)SDL_GetPerformanceCounter()/SDL_GetPerformanceFrequency()*1000;
}

void fullScreen(){
    winFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    width = displayWidth;
    height = displayHeight;
}

void background(int red, int green, int blue){
    SDL_FillRect(surface, &surface->clip_rect, color(red, green, blue));
}

int color(int red, int green, int blue){
    return (0xFF << 24) | (red << 16) | (green << 8) | blue;
}

int red(int col){
    return col >> 16 & 0xFF;
}

int green(int col){
    return col >> 8 & 0xFF;
}

int blue(int col){
    return col & 0xFF;
}

Sound* loadSound(const char* filename){
    return Mix_LoadWAV(filename);
}

void playSound(Sound* sound){
    Mix_PlayChannel(-1, sound, 0);
}

void freeSound(Sound* sound){
    Mix_FreeChunk(sound);
}