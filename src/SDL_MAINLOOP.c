#include "SDL_MAINLOOP.H"
#undef main

#define MAX_NAME  64

unsigned int displayWidth;
unsigned int displayHeight;
unsigned int width = 800;
unsigned int height = 600;
int* pixels;

unsigned int frameRate = 60;
unsigned int frameCount = 0;
float deltaTime;

int pmouseX;
int pmouseY;
int mouseX;
int mouseY;
bool isMousePressed = false;
bool isMouseDragged = false;
button mouseButton;
bool isKeyPressed = false;
bool isKeyReleased = false;
keyboard keyPressed;
keyboard keyReleased;

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

void (*onExit)() = NULL;
void (*onKeyPressed)(keyboard) = NULL;
void (*onKeyReleased)(keyboard) = NULL;

// not accessible variables
SDL_Window* window;
SDL_Surface* surface;
bool running = false;
Uint32 winFlags = SDL_WINDOW_SHOWN;
char windowName[64];

void SDL_MAINLOOP_addAudioHandler(SDL_AudioDeviceID, Uint64, Uint64);
void SDL_MAINLOOP_HandleAudio();

int main(int argc, char* argv[]){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    displayWidth = displayMode.w;
    displayHeight = displayMode.h;
    strcpy(windowName, "window");
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
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
            pmouseX = mouseX;
            pmouseY = mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            if(mouseX < 0)
                mouseX = 0;
            if(mouseY < 0)
                mouseY = 0;
            SDL_Event event;
            isKeyReleased = false;
            isKeyPressed = false;
            while(SDL_PollEvent(&event)){
                switch(event.type){
                    case SDL_WINDOWEVENT:
                    if(event.window.event == SDL_WINDOWEVENT_CLOSE)
                        running = 0;
                    break;

                    case SDL_KEYDOWN:
                    keyPressed = event.key.keysym.sym;
                    isKeyPressed = true;
                    
                    if(event.key.keysym.sym == SDLK_ESCAPE)
                        running = 0;

                    if(onKeyPressed)
                        (*onKeyPressed)(keyPressed);
                    break;

                    case SDL_KEYUP:
                    isKeyReleased = true;
                    keyReleased = event.key.keysym.sym;
                    if(onKeyReleased)
                        (*onKeyReleased)(keyReleased);
                    break;

                    case SDL_MOUSEBUTTONDOWN:
                    isMousePressed = true;
                    mouseButton = event.button.button;
                    break;

                    case SDL_MOUSEBUTTONUP:
                    isMousePressed = false;
                    break;
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

    if(onExit)
        (*onExit)();

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
    if(window){
        SDL_SetWindowTitle(window, windowName);
    }
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