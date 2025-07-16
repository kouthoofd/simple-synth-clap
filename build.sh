#!/bin/bash

# Simple Synth CLAP Build Script for macOS

set -e

echo "Building Simple Synth CLAP..."

# Check if we're on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "This script is designed for macOS"
    exit 1
fi

# Create build directory
mkdir -p build
cd build

# Clone CLAP SDK if not present
if [ ! -d "../clap" ]; then
    echo "Cloning CLAP SDK..."
    cd ..
    git clone --recursive https://github.com/free-audio/clap.git
    cd build
fi

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
echo "Building..."
make -j$(sysctl -n hw.ncpu)

echo "Build complete!"
echo "Plugin location: build/SimpleSynthCLAP.clap"
echo ""
echo "To install, copy the .clap bundle to:"
echo "  ~/Library/Audio/Plug-Ins/CLAP/"
echo ""
echo "Or run: ./install.sh"