# Lid Pong - MacBook Lid Angle Game
# Simple Makefile for building the game

# Compiler and flags
CXX = clang++
CXXFLAGS = -std=c++14 -Wall -Wextra -O2
INCLUDES = -I../mac-angle -I/opt/homebrew/include
LIBS = -framework OpenGL -framework Cocoa -framework IOKit -L/opt/homebrew/lib -lglfw

# Source files
SOURCES = src/LidPong.cpp src/Sensor.cpp ../mac-angle/angle.cpp
TARGET = lid-pong

# Default target
all: $(TARGET)

# Build the game
$(TARGET): $(SOURCES)
	@echo "Building Lid Pong..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SOURCES) -o $(TARGET) $(LIBS)
	@echo "Build complete! Run with: ./$(TARGET)"

# Clean build files
clean:
	@echo "Cleaning build files..."
	rm -f $(TARGET)
	@echo "Clean complete!"

# Run the game
run: $(TARGET)
	@echo "Starting Lid Pong..."
	./$(TARGET)

# Install dependencies (macOS with Homebrew)
install-deps:
	@echo "Installing dependencies..."
	brew install glfw
	@echo "Dependencies installed!"

# Help
help:
	@echo "Lid Pong - MacBook Lid Angle Game"
	@echo "================================="
	@echo "Available targets:"
	@echo "  all          - Build the game (default)"
	@echo "  clean        - Remove build files"
	@echo "  run          - Build and run the game"
	@echo "  install-deps - Install required dependencies"
	@echo "  help         - Show this help message"

.PHONY: all clean run install-deps help