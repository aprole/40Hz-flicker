CC = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
LDFLAGS = -lSDL2
INCLUDES = -I/opt/homebrew/opt/sdl2/include
LIBRARY_PATHS = -L/opt/homebrew/opt/sdl2/lib

TARGET = 40hz_genus
SOURCE = main.cpp

DISPLAY_TEST_TARGET = display_test
DISPLAY_TEST_SOURCE = display_test.cpp

.PHONY: all clean run install-deps help display_test

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CXXFLAGS) $(INCLUDES) $(LIBRARY_PATHS) -o $@ $< $(LDFLAGS)

$(DISPLAY_TEST_TARGET): $(DISPLAY_TEST_SOURCE)
	$(CC) $(CXXFLAGS) $(INCLUDES) $(LIBRARY_PATHS) -o $@ $< $(LDFLAGS)

clean:
	rm -f $(TARGET) $(DISPLAY_TEST_TARGET)

run: $(TARGET)
	./$(TARGET)

install-deps:
	brew install sdl2

help:
	@echo "Available targets:"
	@echo "  make all     - Build the project"
	@echo "  make clean   - Clean compiled files"
	@echo "  make run     - Build and run the project"
	@echo "  make display_test - Build display test"
	@echo "  make install-deps - Install dependencies (SDL2)"
