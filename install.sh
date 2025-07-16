#!/bin/bash

# Install script for Simple Synth CLAP

set -e

PLUGIN_DIR="$HOME/Library/Audio/Plug-Ins/CLAP"
PLUGIN_NAME="SimpleSynthCLAP.clap"

# Check if plugin was built
if [ ! -d "build/$PLUGIN_NAME" ]; then
    echo "Plugin not found. Please run ./build.sh first."
    exit 1
fi

# Create CLAP directory if it doesn't exist
mkdir -p "$PLUGIN_DIR"

# Copy plugin
echo "Installing $PLUGIN_NAME to $PLUGIN_DIR..."
cp -R "build/$PLUGIN_NAME" "$PLUGIN_DIR/"

echo "Installation complete!"
echo "The plugin should now be available in your DAW."