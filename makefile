release:
	gcc src/c/*.c -Isrc/include/SDL2/include -Lsrc/include/SDL2/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -Wno-incompatible-pointer-types

debug:
	gcc -D DEBUG src/c/*.c -Isrc/include/SDL2/include -Lsrc/include/SDL2/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -Wno-incompatible-pointer-types