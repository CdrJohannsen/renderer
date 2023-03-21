CC = g++
TARGET = main
LDLIBS=-lSDL2
CFLAGS= -Wall

all: $(TARGET)
	make clear
	$(CC) -o $(TARGET) $(TARGET).cpp $(LDLIBS)

run:
	make all
	./$(TARGET)

clear:
	rm $(TARGET)
