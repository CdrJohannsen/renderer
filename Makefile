CC = g++
TARGET = main
OUTPUT = renderer
LDLIBS=-lSDL2 -lGL -lGLEW
CFLAGS= -Wall -g

all:
	make clear
	$(CC) -o $(OUTPUT) $(TARGET).cpp $(LDLIBS) $(CFLAGS)

run:
	make all
	./$(OUTPUT)

clear:
	rm -f $(OUTPUT)
