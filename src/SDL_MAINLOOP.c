#include "SDL_MAINLOOP.h"
#undef main

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

// emscripten doesn't include Mix_LoadWAV and Mix_PlayChannel macros 
#ifdef MAINLOOP_AUDIO
#define Mix_LoadWAV(file) Mix_LoadWAV_RW(SDL_RWFromFile(file, "rb"), 1);
#define Mix_PlayChannel(channel, chunk, loops) Mix_PlayChannelTimed(channel, chunk, loops, -1);
#endif

#endif

#define MAX_NAME  64

unsigned int displayWidth;
unsigned int displayHeight;
int width = 800;
int height = 600;
int* pixels;

float frameRate = 60;
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
button exitButton = SDLK_ESCAPE;

void (*onExit)() = NULL;
void (*onKeyPressed)(keyboard) = NULL;
void (*onKeyReleased)(keyboard) = NULL;

void updateWindowIcon();

// not accessible variables
SDL_Window* window;
SDL_Surface* surface;

bool running = false;

#ifdef MAINLOOP_GL
Uint32 winFlags = SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE;
#else 
Uint32 winFlags = SDL_WINDOW_HIDDEN;
#endif

char windowName[MAX_NAME+1];
char iconPath[MAX_NAME+1];

int main_argc;
char** main_argv;

bool render_every_frame = true;

#ifdef MAINLOOP_GL
SDL_Renderer* renderer = NULL;
SDL_Texture* drawBuffer = NULL;
GLuint globalShader = 0;

float render_width;
float render_height;
int win_width;
int win_height;
float render_ratio;
float win_ratio;
float localX;
float localY;
ScaleMode scale_mode = ANISOTROPIC; 

typedef struct shader_list_t {
    GLuint id;
    char* filename;
    struct shader_list_t* next;
} shader_list_t;
shader_list_t* shader_list = NULL;

PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLVALIDATEPROGRAMPROC glValidateProgram;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM2FPROC glUniform2f;

// utility function for shader loading
GLuint compileProgram(const char*);
GLuint compileShader(const char*, GLuint);

void calculateRescaleVars();
void renderOpenGL();
int filterResize(void*, SDL_Event*);
#endif

#ifdef MAINLOOP_WINDOWS
#include <SDL2/SDL_syswm.h>
#include <windows.h>
#include <shlwapi.h>

char absolutePath[1024];

HWND hwnd = NULL;
HMENU mainMenu = NULL;

typedef struct {
    menuId parent_menu;
    void (*callback)();
    size_t position;
} button_t;
size_t n_button = 0;
button_t* buttons = NULL;

typedef struct {
    HMENU hMenu;
    size_t n_button;
    bool is_radio;
} menu_t;
size_t n_menu = 0;
menu_t* menus = NULL;

HWND getWindowHandler();
void createMainMenu();
void updateButtonVect(void (*callback)(), menuId);
void updateMenuVect(HMENU, bool);

#endif

void mainloop();
void render();

// variables used for run loop at correct framerate
#ifndef __EMSCRIPTEN__
Uint64 a_clock;
Uint64 b_clock;
#else
double a_clock;
double b_clock;

void emscripten_mainloop(){
    float millis_per_frame = 1000.0 / frameRate;
    float elapsed;
    b_clock = emscripten_get_now();
    elapsed = b_clock - a_clock;
    deltaTime += elapsed;
    a_clock = b_clock;
    while(deltaTime >= millis_per_frame){
        b_clock = emscripten_get_now();
        mainloop();
        elapsed = b_clock - a_clock;
        a_clock = b_clock;
        // if mainloop lasted for more than millis_per_frame
        // immediately end to avoid infinite lag!
        if(elapsed > millis_per_frame)
            deltaTime = 0;
        else 
            deltaTime -= millis_per_frame;
    }   
}
#endif

int main(int argc, char* argv[]){
    main_argc = argc;
    main_argv = argv;

    #ifdef MAINLOOP_WINDOWS
    GetModuleFileName(NULL, absolutePath, 1024);
    PathRemoveFileSpec(absolutePath);
    #endif

    SDL_Init(
        SDL_INIT_VIDEO |
        SDL_INIT_AUDIO
        #ifdef MAINLOOP_GL
        | SDL_VIDEO_OPENGL
        #endif
    );
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    displayWidth = displayMode.w;
    displayHeight = displayMode.h;
    strcpy(windowName, "window");
    
    #ifdef MAINLOOP_AUDIO
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    #endif
    
    setup();

    window = SDL_CreateWindow(windowName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, winFlags);
    updateWindowIcon();

    #ifdef MAINLOOP_GL
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

    drawBuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
	setScaleMode(scale_mode);
    SDL_SetEventFilter(filterResize, NULL);

    glCreateShader = (PFNGLCREATESHADERPROC)SDL_GL_GetProcAddress("glCreateShader");
	glShaderSource = (PFNGLSHADERSOURCEPROC)SDL_GL_GetProcAddress("glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC)SDL_GL_GetProcAddress("glCompileShader");
	glGetShaderiv = (PFNGLGETSHADERIVPROC)SDL_GL_GetProcAddress("glGetShaderiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)SDL_GL_GetProcAddress("glGetShaderInfoLog");
	glDeleteShader = (PFNGLDELETESHADERPROC)SDL_GL_GetProcAddress("glDeleteShader");
	glAttachShader = (PFNGLATTACHSHADERPROC)SDL_GL_GetProcAddress("glAttachShader");
	glCreateProgram = (PFNGLCREATEPROGRAMPROC)SDL_GL_GetProcAddress("glCreateProgram");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)SDL_GL_GetProcAddress("glLinkProgram");
	glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)SDL_GL_GetProcAddress("glValidateProgram");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)SDL_GL_GetProcAddress("glGetProgramiv");
	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)SDL_GL_GetProcAddress("glGetProgramInfoLog");
	glUseProgram = (PFNGLUSEPROGRAMPROC)SDL_GL_GetProcAddress("glUseProgram");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)SDL_GL_GetProcAddress("glGetUniformLocation");
	glUniform1f = (PFNGLUNIFORM1FPROC)SDL_GL_GetProcAddress("glUniform1f");
    glUniform2f = (PFNGLUNIFORM2FPROC)SDL_GL_GetProcAddress("glUniform2f");

    shader_list_t* p = shader_list;
    while(p){
        p->id = compileProgram(p->filename);
        free(p->filename);
        p = p->next;
    } 
    #endif

    #ifdef MAINLOOP_WINDOWS
    hwnd = getWindowHandler();

    if(mainMenu && !(winFlags & SDL_WINDOW_FULLSCREEN_DESKTOP))
        SetMenu(hwnd, mainMenu);

    SDL_SetWindowSize(window, width, height);
    #endif

    SDL_ShowWindow(window);

    #ifndef MAINLOOP_GL
    surface = SDL_GetWindowSurface(window);
    pixels = (int*)surface->pixels;
    width = surface->w;
    height = surface->h;
    #endif

    deltaTime = 0;
    #ifdef __EMSCRIPTEN__
    a_clock = emscripten_get_now();
    b_clock = emscripten_get_now();
    emscripten_set_main_loop(emscripten_mainloop, 0, 1);
    #else 
    a_clock = SDL_GetPerformanceCounter();
    b_clock = SDL_GetPerformanceCounter();

    running = true;
    while(running){
        a_clock = SDL_GetPerformanceCounter();
        deltaTime = (float)(a_clock - b_clock)/SDL_GetPerformanceFrequency()*1000;

        if(deltaTime > 1000.0f / frameRate){
            mainloop();

            b_clock = a_clock;
        } else {
            float ms = 1000.0f/frameRate;
            if(ms - deltaTime > 1.0f)
                SDL_Delay(ms - deltaTime - 1);
        }
    };
    #endif

    if(onExit)
        (*onExit)();

    #ifdef MAINLOOP_AUDIO
    Mix_CloseAudio();
    #endif

    #ifdef MAINLOOP_GL
    SDL_DestroyTexture(drawBuffer);
	SDL_DestroyRenderer(renderer);
    
    while(shader_list){
        shader_list_t* tmp = shader_list->next;
        free(shader_list);
        shader_list = tmp;
    }
    #endif

    #ifdef MAINLOOP_WINDOWS
    free(buttons);
    free(menus);
    #endif

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void mainloop(){
    frameCount++;

    #ifdef MAINLOOP_WINDOWS
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {   
        if(msg.message == WM_COMMAND){
            unsigned int button_id = LOWORD(msg.wParam); 
            if(button_id < n_button){
                checkRadioButton(button_id);
                if(buttons[button_id].callback)
                    (*buttons[button_id].callback)();
            }
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    #endif

    pmouseX = mouseX;
    pmouseY = mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    #ifdef MAINLOOP_GL
    calculateRescaleVars();
    mouseX -= localX;
    mouseY -= localY;
    mouseX *= width/render_width;
    mouseY *= height/render_height;
    #endif
    if(mouseX < 0)
        mouseX = 0;
    if(mouseY < 0)
        mouseY = 0;
    if(mouseX >= width)
        mouseX = width-1;
    if(mouseY >= height)
        mouseY = height-1;
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
            
            if(event.key.keysym.sym == exitButton)
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

    #ifdef MAINLOOP_GL
    SDL_LockTextureToSurface(drawBuffer, NULL, &surface);
    pixels = (int*)surface->pixels;
    #endif

    loop();

    if(render_every_frame)
        render();
}

void render(){
    #ifdef MAINLOOP_GL
    SDL_UnlockTexture(drawBuffer);  
    renderOpenGL();
    #else 
    SDL_UpdateWindowSurface(window);
    #endif
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

void setWindowIcon(const char* filename){
    strncpy(iconPath, filename, MAX_NAME);
    if(window)
        updateWindowIcon();
}

void updateWindowIcon(){
    SDL_Surface* icon = SDL_LoadBMP(iconPath);
    SDL_SetWindowIcon(window, icon);
    SDL_FreeSurface(icon);
}

Uint64 millis(){
    return (float)SDL_GetPerformanceCounter()/SDL_GetPerformanceFrequency()*1000;
}

void fullScreen(){
    #ifdef MAINLOOP_WINDOWS
    if(hwnd && window && mainMenu){
        if(winFlags & SDL_WINDOW_FULLSCREEN_DESKTOP)
            SetMenu(hwnd, mainMenu);
        else
            SetMenu(hwnd, NULL);
        SDL_SetWindowSize(window, width, height);
    }
    #endif

    if(!(winFlags & SDL_WINDOW_FULLSCREEN_DESKTOP))
        winFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    else 
        winFlags &= ~(SDL_WINDOW_FULLSCREEN_DESKTOP);
    
    if(window)
        SDL_SetWindowFullscreen(window, winFlags & SDL_WINDOW_FULLSCREEN_DESKTOP);

    #ifndef MAINLOOP_GL
    SDL_GetWindowSize(window, &width, &height);
    surface = SDL_GetWindowSurface(window);
    pixels = (int*)surface->pixels;
    #endif
}

void background(int red, int green, int blue){
    SDL_FillRect(surface, &surface->clip_rect, color(red, green, blue));
}

int color(int red, int green, int blue){
    return SDL_MapRGB(surface->format, red, green, blue);
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

int getArgc(){
    return main_argc;
}

const char* getArgv(int idx){
    if(idx >= main_argc)
        return NULL;
    else
        return main_argv[idx];
}

void noRender(){
    render_every_frame = false;
}

void autoRender(){
    render_every_frame = true;
}

void renderPixels(){
    render();
    #ifdef MAINLOOP_GL
    SDL_LockTextureToSurface(drawBuffer, NULL, &surface);
    pixels = (int*)surface->pixels;
    #endif
}

#ifdef MAINLOOP_AUDIO

Sound* loadSound(const char* filename){
    return Mix_LoadWAV(filename);
}

void playSound(Sound* sound){
    Mix_PlayChannel(-1, sound, 0);
}

void freeSound(Sound* sound){
    Mix_FreeChunk(sound);
}

#endif

#ifdef MAINLOOP_GL
GLuint compileShader(const char* source, GLuint shaderType) {
	GLuint result = glCreateShader(shaderType);
	glShaderSource(result, 1, &source, NULL);
	glCompileShader(result);

	//Check vertex shader for errors
	GLint shaderCompiled = GL_FALSE;
	glGetShaderiv( result, GL_COMPILE_STATUS, &shaderCompiled );
	if( shaderCompiled != GL_TRUE ) {
		fprintf(stderr, "Shader compilation error %d!\n", result);
		GLint logLength;
		glGetShaderiv(result, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 0)
		{
			GLchar *log = (GLchar*)malloc(logLength);
			glGetShaderInfoLog(result, logLength, &logLength, log);
			fprintf(stdout, "compield Shader log:\n%s\n", log);
			free(log);
		}
		glDeleteShader(result);
		result = 0;
	}
	return result;
}

GLuint compileProgram(const char* fragFile) {
	GLuint programId = 0;
	GLuint fragShaderId;

	programId = glCreateProgram();

	FILE* fptr = fopen(fragFile, "rb");
	fseek(fptr, 0, SEEK_END);
	size_t size = ftell(fptr);
	rewind(fptr);
	char* fcontent = (char*)malloc(size+1);
	fread(fcontent, 1, size, fptr);
    fcontent[size] = '\0';

	fragShaderId = compileShader(fcontent, GL_FRAGMENT_SHADER);
	
	if(fragShaderId) {
		glAttachShader(programId, fragShaderId);
		glLinkProgram(programId);
		glValidateProgram(programId);

		GLint logLen;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLen);
		if(logLen > 0) {
			char* log = (char*) malloc(logLen * sizeof(char));

			glGetProgramInfoLog(programId, logLen, &logLen, log);
			fprintf(stdout, "Prog Info Log: \n%s\n", log);
			free(log);
		}
	}
	if(fragShaderId) {
		glDeleteShader(fragShaderId);
	}
	return programId;
}

Shader loadShader(const char* filename){
    shader_list_t* block = (shader_list_t*)malloc(sizeof(shader_list_t));
    block->next = NULL;
    block->filename = (char*)malloc(129);
    strncpy(block->filename, filename, 128);
    if(!shader_list){
        shader_list = block;
    } else {
        block->next = shader_list;
        shader_list = block;
    }
    return &block->id;
}

void noGlobalShader(){
    globalShader = 0;
}

void setGlobalShader(Shader sh){
    shader_list_t* p = shader_list;
    while(p->id != *sh)
        p = p->next;
    globalShader = p->id; 
}

void setScaleMode(ScaleMode mode){
    if(!renderer){
        scale_mode = mode;
        return;    
    }

    switch(mode){
        case NEAREST:
        SDL_SetTextureScaleMode(drawBuffer, SDL_ScaleModeNearest);
        break;

        case LINEAR:
        SDL_SetTextureScaleMode(drawBuffer, SDL_ScaleModeLinear);
        break;

        case ANISOTROPIC:
        SDL_SetTextureScaleMode(drawBuffer, SDL_ScaleModeBest);
        break;
    }
}

void setVoidColor(int r, int g, int b){
    glClearColor(r/255.0f, g/255.0f, b/255.0f, 1.0f);
}

void calculateRescaleVars(){
    SDL_GetWindowSize(window, &win_width, &win_height);
    render_ratio = (float)width/height;
    win_ratio = (float)win_width/win_height;
    if(win_ratio > render_ratio){
        render_width = (float)width*win_height/height;
        render_height = win_height;
    } else {
        render_width = win_width;
        render_height = height*win_width/width;
    }
    localX = win_width/2-render_width/2;
    localY = win_height/2-render_height/2;
}

void renderOpenGL(){
    SDL_RenderClear(renderer);
    SDL_GL_BindTexture(drawBuffer, NULL, NULL);
    glUseProgram(globalShader);

    glViewport(win_width/2-render_width/2, win_height/2-render_height/2, render_width, render_height);
    glUniform1f(glGetUniformLocation(globalShader, "width"), render_width);
    glUniform1f(glGetUniformLocation(globalShader, "height"), render_height);
    glUniform1f(glGetUniformLocation(globalShader, "localX"), localX);
    glUniform1f(glGetUniformLocation(globalShader, "localY"), localY);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(-1, 1);
        glTexCoord2f(1, 0); glVertex2f(1, 1);
        glTexCoord2f(1, 1); glVertex2f(1, -1);
        glTexCoord2f(0, 1); glVertex2f(-1, -1);
    glEnd();
    SDL_RenderPresent(renderer);
}

int filterResize(void* userdata, SDL_Event* event){
    if(event->type == SDL_WINDOWEVENT)
        if(event->window.event == SDL_WINDOWEVENT_RESIZED || event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
            calculateRescaleVars();
            renderOpenGL();
            return 0;
        }

    return 1;
}

#endif

#ifdef MAINLOOP_WINDOWS
void getAbsoluteDir(char* dst){
    strcpy(dst, absolutePath);
    strcat(dst, "\\");
}

HWND getWindowHandler(){
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window, &wmInfo);
    return wmInfo.info.win.window;
}

void createMainMenu(){
    if(!mainMenu)
        mainMenu = CreateMenu();
}

void updateButtonVect(void (*callback)(), menuId parentMenu){
    if(!buttons)
        buttons = (button_t*)malloc(sizeof(button_t));
    else
        buttons = (button_t*)realloc(buttons, (n_button+1)*sizeof(button_t));
    buttons[n_button].callback = callback;
    buttons[n_button].parent_menu = parentMenu;
    if(parentMenu != -1){
      buttons[n_button].position = menus[parentMenu].n_button-1;  
    }
    n_button++;
}

void updateMenuVect(HMENU new_menu, bool isRadio){
    if(!menus)
        menus = (menu_t*)malloc(sizeof(menu_t));
    else
        menus = (menu_t*)realloc(menus, (n_menu+1)*sizeof(menu_t));
    menus[n_menu].hMenu = new_menu;
    menus[n_menu].n_button = 0;
    menus[n_menu].is_radio = isRadio;
    n_menu++;
}

menuId addMenuTo(menuId parentId, const wchar_t* string, bool isRadio){
    HMENU parent = NULL;
    if(parentId < n_menu)
        parent = menus[parentId].hMenu;
    if(!parent){
        createMainMenu();
        parent = mainMenu;
    }
    HMENU new_menu = CreateMenu();
    AppendMenuW(parent, MF_POPUP, (UINT_PTR) new_menu, string);
    updateMenuVect(new_menu, isRadio);
    return n_menu-1;
}

buttonId addButtonTo(menuId parentId, const wchar_t* string, void (*callback)()){
    HMENU parent = NULL;
    if(parentId < n_menu){
        parent = menus[parentId].hMenu;
        menus[parentId].n_button++;
    }
    if(!parent){
        createMainMenu();
        parent = mainMenu;
    }
    AppendMenuW(parent, MF_STRING, n_button, string);
    updateButtonVect(callback, parentId);
    return n_button-1;
}

void checkRadioButton(buttonId button_id){
    if(button_id < n_button){
        menuId menu_id = buttons[button_id].parent_menu;
        if(menu_id < n_menu && menus[menu_id].is_radio)
            CheckMenuRadioItem(menus[menu_id].hMenu, 0, menus[menu_id].n_button-1, buttons[button_id].position, MF_BYPOSITION);
    }
}
#endif