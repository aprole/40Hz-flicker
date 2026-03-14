CC = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
LDFLAGS = -lSDL2
INCLUDES = -I/opt/homebrew/opt/sdl2/include
LIBRARY_PATHS = -L/opt/homebrew/opt/sdl2/lib

TARGET = 40hz_genus
SOURCE = main.cpp

.PHONY: all clean run install-deps help

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CXXFLAGS) $(INCLUDES) $(LIBRARY_PATHS) -o $@ $< $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

install-deps:
	brew install sdl2

help:
	@echo "Available targets:"
	@echo "  make all     - Build the project"
	@echo "  make clean   - Clean compiled files"
	@echo "  make run     - Build and run the project"
	@echo "  make install-deps - Install dependencies (SDL2)"
