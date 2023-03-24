CC = g++
TARGET = main
LDLIBS=-lSDL2
CFLAGS= -Wall -g

all: $(TARGET)
	make clear
	$(CC) -o $(TARGET) $(TARGET).cpp $(LDLIBS) $(CFLAGS)

run:
	make all
	./$(TARGET)

clear:
	rm $(TARGET)
