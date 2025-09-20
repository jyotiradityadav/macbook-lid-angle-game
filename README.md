# Lid Pong 🎮

A unique Pong-style game that uses your MacBook's lid angle sensor as the primary control mechanism. Tilt your MacBook lid to control the paddle and keep the ball in play!

## Features ✨

- **Innovative Control**: Use your MacBook's lid angle to control the game paddle
- **Real-time Physics**: Smooth ball movement with realistic collision detection
- **Adjustable Speed**: Customize ball speed with mouse slider or keyboard
- **Lives System**: 3 lives with visual indicators
- **Score Tracking**: Count your consecutive hits
- **Game Over Modal**: Clear game over screen with restart option
- **Cross-platform Ready**: Designed for easy deployment

## Screenshots 📸

*Game in action with lid angle control and clean UI*

## Requirements 📋

### System Requirements
- **macOS** (for lid angle sensor support)
- **OpenGL 3.3+** compatible graphics
- **GLFW** library for window management

### Dependencies
- GLFW (for window and input handling)
- OpenGL (system provided)
- IOKit framework (system provided)

## Quick Start 🚀

### Option 1: Using Homebrew (Recommended)

```bash
# Install dependencies
brew install glfw

# Clone and build
git clone <repository-url>
cd lid-pong
make

# Run the game
./lid-pong
```

### Option 2: Manual Installation

```bash
# Install GLFW manually or use package manager
# Then build
make install-deps  # Install dependencies
make              # Build the game
make run          # Run the game
```

### Option 3: Docker (Coming Soon)

```bash
# Docker support for easy deployment
docker build -t lid-pong .
docker run -it --rm lid-pong
```

## How to Play 🎯

### Controls
- **MacBook Lid**: Tilt your lid up/down to move the paddle
- **Mouse**: Drag the speed slider at the bottom to adjust ball speed
- **+/- Keys**: Fine-tune ball speed
- **SPACE**: Reset ball position or restart game
- **ESC**: Quit the game

### Gameplay
1. **Start the game** - The ball will begin moving automatically
2. **Control the paddle** - Use your MacBook lid angle to move the paddle up and down
3. **Keep the ball in play** - Prevent the ball from going off the left side
4. **Score points** - Each time you hit the ball with the paddle, your score increases
5. **Manage lives** - You have 3 lives; lose one each time you miss the ball
6. **Game over** - When all lives are lost, press SPACE to restart

### Tips
- **Small lid movements** create large paddle movements (high sensitivity)
- **Adjust ball speed** to match your skill level
- **Practice** lid control for better precision

## Building from Source 🔨

### Prerequisites
```bash
# macOS with Xcode Command Line Tools
xcode-select --install

# Homebrew (recommended)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

### Build Steps
```bash
# 1. Clone the repository
git clone <repository-url>
cd lid-pong

# 2. Install dependencies
make install-deps

# 3. Build the game
make

# 4. Run
./lid-pong
```

### Build Options
```bash
make help          # Show all available commands
make clean         # Clean build files
make run           # Build and run in one command
```

## Project Structure 📁

```
lid-pong/
├── src/
│   ├── LidPong.cpp     # Main game implementation
│   ├── Sensor.cpp      # Lid angle sensor wrapper
│   └── Sensor.h        # Sensor interface
├── mac-angle/          # Lid angle sensor library
│   ├── angle.cpp
│   ├── angle.h
│   └── ...
├── Makefile           # Build configuration
├── README.md          # This file
├── CONTRIBUTING.md    # Contribution guidelines
└── Dockerfile         # Docker configuration
```

## Technical Details 🔧

### Architecture
- **C++14** standard for compatibility
- **OpenGL** for graphics rendering
- **GLFW** for window management and input
- **IOKit** for hardware sensor access

### Performance
- **60 FPS** target frame rate
- **Real-time** lid angle reading
- **Optimized** collision detection
- **Smooth** ball physics

### Compatibility
- **macOS 10.12+** (for lid sensor support)
- **Intel and Apple Silicon** Macs
- **Retina and non-Retina** displays

## Troubleshooting 🔧

### Common Issues

**Game won't start:**
```bash
# Check dependencies
brew list glfw
# Reinstall if missing
brew install glfw
```

**Lid sensor not working:**
- Ensure you're running on a MacBook with lid angle sensor
- Check console output for sensor initialization messages
- Use keyboard controls (UP/DOWN arrows) as fallback

**Build errors:**
```bash
# Clean and rebuild
make clean
make
```

**Performance issues:**
- Adjust ball speed to lower values
- Close other applications
- Check OpenGL compatibility



---

