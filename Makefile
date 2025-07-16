# Simple Synth CLAP Makefile for macOS

CXX = clang++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -fPIC
OBJCFLAGS = -std=c++17 -O2 -Wall -Wextra -fPIC -fobjc-arc

# Directories
SRC_DIR = src
BUILD_DIR = build
CLAP_DIR = clap

# Include paths
INCLUDES = -I$(CLAP_DIR)/include -I$(SRC_DIR)

# Frameworks for macOS
FRAMEWORKS = -framework Cocoa -framework CoreGraphics

# Source files
CPP_SOURCES = $(SRC_DIR)/simple_synth.cpp $(SRC_DIR)/voice.cpp $(SRC_DIR)/plugin.cpp
# MM_SOURCES = $(SRC_DIR)/ui.mm  # Disabled for now
MM_SOURCES =

# Object files
CPP_OBJECTS = $(CPP_SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
MM_OBJECTS = $(MM_SOURCES:$(SRC_DIR)/%.mm=$(BUILD_DIR)/%.o)
OBJECTS = $(CPP_OBJECTS) $(MM_OBJECTS)

# Target
TARGET = $(BUILD_DIR)/SimpleSynthCLAP.clap
BUNDLE_BINARY = $(TARGET)/Contents/MacOS/SimpleSynthCLAP

.PHONY: all clean install

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BUILD_DIR)
	@echo "Creating CLAP bundle..."
	mkdir -p $(TARGET)/Contents/MacOS
	mkdir -p $(TARGET)/Contents/Resources
	$(CXX) -bundle -o $(BUNDLE_BINARY) $(OBJECTS) $(FRAMEWORKS)
	cp Info.plist $(TARGET)/Contents/Info.plist
	@echo "Build complete: $@"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.mm | $(BUILD_DIR)
	$(CXX) $(OBJCFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

install: $(TARGET)
	mkdir -p ~/Library/Audio/Plug-Ins/CLAP
	rm -rf ~/Library/Audio/Plug-Ins/CLAP/SimpleSynthCLAP.clap
	cp -R $(TARGET) ~/Library/Audio/Plug-Ins/CLAP/
	@echo "Plugin installed to ~/Library/Audio/Plug-Ins/CLAP/"

# Check if CLAP SDK exists
check-clap:
	@if [ ! -d "$(CLAP_DIR)" ]; then \
		echo "CLAP SDK not found. Cloning..."; \
		git clone --recursive https://github.com/free-audio/clap.git; \
	fi

$(OBJECTS): | check-clap