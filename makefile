# Compiler and compiler flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall

# SDL2 specific paths and flags
SDL2_PATH = /usr/local/opt/sdl2
SDL2_CFLAGS = -I$(SDL2_PATH)/include/SDL2 -D_THREAD_SAFE
SDL2_LDFLAGS = -L$(SDL2_PATH)/lib -lSDL2

# Target executable
TARGET = chip8 

# Source files
SRCS = chip8.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Link the target executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(SDL2_LDFLAGS)

# Compile the source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(SDL2_CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(TARGET) $(OBJS)

# Phony targets
.PHONY: all clean

