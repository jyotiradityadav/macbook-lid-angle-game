#!/bin/bash

# Lid Pong - Release Build Script
# Creates optimized, distributable executables

set -e  # Exit on any error

echo "🎮 Lid Pong - Release Build Script"
echo "=================================="

# Configuration
APP_NAME="LidPong"
VERSION="1.0.0"
BUILD_DIR="release"
DIST_DIR="dist"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Helper functions
log_info() {
    echo -e "${BLUE}ℹ️  $1${NC}"
}

log_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

log_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

log_error() {
    echo -e "${RED}❌ $1${NC}"
}

# Check dependencies
check_dependencies() {
    log_info "Checking dependencies..."
    
    if ! command -v clang++ &> /dev/null; then
        log_error "clang++ not found. Please install Xcode Command Line Tools."
        exit 1
    fi
    
    # Check for GLFW in common locations
    GLFW_FOUND=false
    if [ -f "/opt/homebrew/include/GLFW/glfw3.h" ] || [ -f "/usr/local/include/GLFW/glfw3.h" ] || [ -f "/usr/include/GLFW/glfw3.h" ]; then
        GLFW_FOUND=true
    fi
    
    if [ "$GLFW_FOUND" = false ]; then
        log_warning "GLFW not found in standard locations. Attempting to build anyway..."
        log_info "If build fails, install GLFW with: brew install glfw"
    else
        log_success "GLFW found"
    fi
    
    log_success "Dependencies check complete"
}

# Clean previous builds
clean_build() {
    log_info "Cleaning previous builds..."
    rm -rf "$BUILD_DIR" "$DIST_DIR" lid-pong
    mkdir -p "$BUILD_DIR" "$DIST_DIR"
    log_success "Build directories cleaned"
}

# Build optimized executable
build_executable() {
    log_info "Building optimized executable..."
    
    # Compiler flags for release build
    CXXFLAGS="-std=c++14 -O3 -DNDEBUG -Wall -Wextra -flto"
    
    # Flexible include paths
    INCLUDES="-I../mac-angle"
    if [ -d "/opt/homebrew/include" ]; then
        INCLUDES="$INCLUDES -I/opt/homebrew/include"
    elif [ -d "/usr/local/include" ]; then
        INCLUDES="$INCLUDES -I/usr/local/include"
    fi
    
    # Flexible library paths
    LIBS="-framework OpenGL -framework Cocoa -framework IOKit"
    if [ -d "/opt/homebrew/lib" ]; then
        LIBS="$LIBS -L/opt/homebrew/lib -lglfw"
    elif [ -d "/usr/local/lib" ]; then
        LIBS="$LIBS -L/usr/local/lib -lglfw"
    else
        LIBS="$LIBS -lglfw"
    fi
    
    SOURCES="src/LidPong.cpp src/Sensor.cpp ../mac-angle/angle.cpp"
    
    # Build with optimization
    clang++ $CXXFLAGS $INCLUDES $SOURCES -o "$BUILD_DIR/$APP_NAME" $LIBS
    
    # Strip debug symbols for smaller size
    strip "$BUILD_DIR/$APP_NAME"
    
    log_success "Executable built successfully"
}

# Create app bundle (macOS)
create_app_bundle() {
    log_info "Creating macOS app bundle..."
    
    BUNDLE_DIR="$DIST_DIR/$APP_NAME.app"
    CONTENTS_DIR="$BUNDLE_DIR/Contents"
    MACOS_DIR="$CONTENTS_DIR/MacOS"
    RESOURCES_DIR="$CONTENTS_DIR/Resources"
    
    # Create bundle structure
    mkdir -p "$MACOS_DIR" "$RESOURCES_DIR"
    
    # Copy executable
    cp "$BUILD_DIR/$APP_NAME" "$MACOS_DIR/"
    
    # Create Info.plist
    cat > "$CONTENTS_DIR/Info.plist" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>$APP_NAME</string>
    <key>CFBundleIdentifier</key>
    <string>com.lidpong.game</string>
    <key>CFBundleName</key>
    <string>$APP_NAME</string>
    <key>CFBundleVersion</key>
    <string>$VERSION</string>
    <key>CFBundleShortVersionString</key>
    <string>$VERSION</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>LSMinimumSystemVersion</key>
    <string>10.12</string>
    <key>NSHighResolutionCapable</key>
    <true/>
    <key>LSApplicationCategoryType</key>
    <string>public.app-category.games</string>
</dict>
</plist>
EOF
    
    # Copy resources (from parent directory since they were moved)
    if [ -f "../README.md" ]; then
        cp ../README.md "$RESOURCES_DIR/"
    fi
    if [ -f "../CONTRIBUTING.md" ]; then
        cp ../CONTRIBUTING.md "$RESOURCES_DIR/"
    fi
    
    log_success "App bundle created: $BUNDLE_DIR"
}

# Create distributable archive
create_distribution() {
    log_info "Creating distribution archive..."
    
    cd "$DIST_DIR"
    
    # Create ZIP archive
    zip -r "$APP_NAME-v$VERSION-macOS.zip" "$APP_NAME.app"
    
    # Create standalone executable ZIP
    mkdir -p "standalone"
    cp "../$BUILD_DIR/$APP_NAME" "standalone/"
    if [ -f "../README.md" ]; then
        cp ../README.md "standalone/"
    fi
    if [ -f "../CONTRIBUTING.md" ]; then
        cp ../CONTRIBUTING.md "standalone/"
    fi
    zip -r "$APP_NAME-v$VERSION-standalone.zip" "standalone/"
    rm -rf "standalone"
    
    cd ..
    
    log_success "Distribution archives created in $DIST_DIR/"
}

# Generate checksums
generate_checksums() {
    log_info "Generating checksums..."
    
    cd "$DIST_DIR"
    shasum -a 256 *.zip > checksums.txt
    cd ..
    
    log_success "Checksums generated"
}

# Display build info
show_build_info() {
    echo ""
    echo "🎉 Build Complete!"
    echo "=================="
    echo "App Name: $APP_NAME"
    echo "Version: $VERSION"
    echo "Build Directory: $BUILD_DIR/"
    echo "Distribution Directory: $DIST_DIR/"
    echo ""
    echo "📦 Distribution Files:"
    ls -la "$DIST_DIR/"
    echo ""
    echo "📋 File Sizes:"
    du -sh "$DIST_DIR"/*
    echo ""
    echo "🚀 Ready for distribution!"
}

# Main build process
main() {
    check_dependencies
    clean_build
    build_executable
    create_app_bundle
    create_distribution
    generate_checksums
    show_build_info
}

# Handle script arguments
case "${1:-}" in
    "clean")
        log_info "Cleaning build artifacts..."
        rm -rf "$BUILD_DIR" "$DIST_DIR" lid-pong
        log_success "Clean complete"
        ;;
    "help"|"-h"|"--help")
        echo "Usage: $0 [clean|help]"
        echo ""
        echo "Commands:"
        echo "  (no args)  - Build release version"
        echo "  clean      - Clean build artifacts"
        echo "  help       - Show this help message"
        ;;
    "")
        main
        ;;
    *)
        log_error "Unknown command: $1"
        echo "Use '$0 help' for usage information"
        exit 1
        ;;
esac
