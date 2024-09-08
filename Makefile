CC = gcc
CFLAGS = -g -Wall -pedantic -std=c99 -Ilib/glad/include -Ilib/cglm/include

ifeq ($(OS),Windows_NT)
	CFLAGS += -Ilib/SDL2_win/include
	LDFLAGS = lib/glad/src/glad.o -Llib/SDL2_win/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lm
	EXECUTABLE = game.exe
else
	EXECUTABLE = game
	
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		LDFLAGS = lib/glad/src/glad.o -lSDL2 -lSDL2_mixer -lm
	endif
	ifeq ($(UNAME_S),Darwin)
		CFLAGS += -Ilib/SDL2_mac/include
		LDFLAGS = lib/glad/src/glad.o -Llib/SDL2_mac/lib -lSDL2 -lSDL2_mixer -lm
	endif
endif

SRC = src/main.c src/engine/render.c src/engine/util.c src/engine/shader.c src/engine/input.c src/engine/res.c
OBJ = $(SRC:.c=.o)

all: libs game

libs:
	cd lib/glad && $(CC) -o src/glad.o -Iinclude -c src/glad.c

game: $(OBJ)
	$(CC) -o $(EXECUTABLE) $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm $(OBJ) lib/glad/src/glad.o $(EXECUTABLE)