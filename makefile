CC = g++
CFLAGS = -I./include -ID:/raylib/w64devkit/x86_64-w64-mingw32/include
LDFLAGS = -LD:/raylib/w64devkit/x86_64-w64-mingw32/lib -lraylib -lopengl32 -lgdi32 -lwinmm -static

all: BrickBreaker.exe

BrickBreaker.exe: src/*.cpp
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f BrickBreaker.exe