CC = g++
TARGETS = main.cpp shader.cpp imgui/imgui.cpp imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp imgui/backends/imgui_impl_sdl2.cpp imgui/backends/imgui_impl_opengl3.cpp
OUTPUT = renderer
LDLIBS=-lSDL2 -lGL -lGLEW 
CFLAGS= -Wall -g -Iimgui -Iimgui/backends
TEST_MODEL= ./ressources/box.mod
.DEFAULT_GOAL := debug-run

all:
	make clear
	$(CC) -o $(OUTPUT) $(TARGETS) $(LDLIBS) $(CFLAGS)

run:
	./$(OUTPUT) $(TEST_MODEL)

clear:
	rm -f $(OUTPUT)

debug:
	$(CC) -o $(OUTPUT) $(TARGETS) $(LDLIBS) $(CFLAGS) -D _DEBUG

debug-run:
	make debug run
