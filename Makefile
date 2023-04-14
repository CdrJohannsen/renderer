CC = g++
TARGETS = main.cpp shader.cpp
OUTPUT = renderer
LDLIBS=-lSDL2 -lGL -lGLEW
CFLAGS= -Wall -g

all:
	make clear
	$(CC) -o $(OUTPUT) $(TARGETS) $(LDLIBS) $(CFLAGS)

run:
	./$(OUTPUT)

clear:
	rm -f $(OUTPUT)

debug:
	$(CC) -o $(OUTPUT) $(TARGETS) $(LDLIBS) $(CFLAGS) -D _DEBUG
