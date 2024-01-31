release:
	gcc src/*.c -Iinclude -Llib -lSDL2 -lSDL2_mixer -lopengl32 -D MAINLOOP_AUDIO -D MAINLOOP_GL

debug:
	gcc -D DEBUG src/*.c -Iinclude -Llib -lSDL2 -lSDL2_mixer -D MAINLOOP_AUDIO

emcc:
	emcc -Iinclude $(wildcard src/*.c) -O2 \
	--preload-file ROM \
	-sUSE_SDL=2 -sUSE_SDL_MIXER=2 \
	-o website/emulator.html