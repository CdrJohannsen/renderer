CC = g++
TARGETS = modelexporter.cpp
OUTPUT = exporter
LDLIBS=-lassimp
CFLAGS= -Wall -g

all:
	make clear
	$(CC) -o $(OUTPUT) $(TARGETS) $(LDLIBS) $(CFLAGS)

run:
	./$(OUTPUT) ../ressources/box.fbx

clear:
	rm -f $(OUTPUT)

debug:
	$(CC) -o $(OUTPUT) $(TARGETS) $(LDLIBS) $(CFLAGS) -D _DEBUG
