#!/bin/bash

# Build script for MacBook Lid Angle Sensor C++ Library

set -e

echo "=== MacBook Lid Angle Sensor C++ Library Build Script ==="
echo

# Check if we're on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "❌ Error: This library only supports macOS"
    exit 1
fi

# Check for required tools
echo "Checking dependencies..."

if ! command -v cmake &> /dev/null; then
    echo "❌ Error: CMake is required but not installed"
    echo "   Install with: brew install cmake"
    exit 1
fi

if ! command -v clang++ &> /dev/null; then
    echo "❌ Error: Clang++ is required but not installed"
    echo "   Install Xcode command line tools with: xcode-select --install"
    exit 1
fi

echo "✅ All dependencies found"
echo

# Create build directory
BUILD_DIR="build"
if [[ -d "$BUILD_DIR" ]]; then
    echo "🔄 Cleaning existing build directory..."
    rm -rf "$BUILD_DIR"
fi

mkdir "$BUILD_DIR"
cd "$BUILD_DIR"

echo "📦 Configuring build with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

echo
echo "🔨 Building library and example..."
make -j$(sysctl -n hw.ncpu)

echo
echo "✅ Build completed successfully!"
echo

# Check if example was built
if [[ -f "lid_angle_example" ]]; then
    echo "🎯 Example program built successfully"
    echo "   Run with: ./build/lid_angle_example"
    echo "   Run continuous demo: ./build/lid_angle_example --continuous"
else
    echo "⚠️  Example program not found"
fi

echo
echo "📚 Library files created:"
if [[ -f "libmacbook_lid_angle.a" ]]; then
    echo "   ✅ Static library: $(pwd)/libmacbook_lid_angle.a"
else
    echo "   ❌ Static library not found"
fi

echo
echo "🚀 Build completed! You can now:"
echo "   1. Run the example: ./build/lid_angle_example"
echo "   2. Install the library: cd build && make install"
echo "   3. Link against the library in your projects"
