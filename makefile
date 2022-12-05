release:
	gcc src/*.c -Iinclude -Llib -lSDL2main -lSDL2 -lSDL2_mixer -Wno-incompatible-pointer-types

debug:
	gcc -D DEBUG src/*.c -Iinclude -Llib -lSDL2main -lSDL2 -lSDL2_mixer -Wno-incompatible-pointer-types