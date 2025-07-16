cmake_minimum_required(VERSION 3.20)
project(SimpleSynthCLAP VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)

# CLAP SDK
add_subdirectory(clap EXCLUDE_FROM_ALL)

# Plugin source files
set(PLUGIN_SOURCES
    src/simple_synth.cpp
    src/simple_synth.h
    src/voice.cpp
    src/voice.h
    src/plugin.cpp
    src/ui.h
)

# Platform-specific UI sources (disabled for now)
# if(APPLE)
#     list(APPEND PLUGIN_SOURCES src/ui.mm)
# endif()

# Create the plugin
add_library(SimpleSynthCLAP MODULE ${PLUGIN_SOURCES})

# Link CLAP
target_link_libraries(SimpleSynthCLAP PRIVATE clap-core)
target_include_directories(SimpleSynthCLAP PRIVATE clap/include)

# macOS specific settings
if(APPLE)
    set_target_properties(SimpleSynthCLAP PROPERTIES
        BUNDLE TRUE
        BUNDLE_EXTENSION "clap"
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
    )
    
    # Link macOS frameworks for UI
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(COREGRAPHICS_FRAMEWORK CoreGraphics)
    target_link_libraries(SimpleSynthCLAP PRIVATE 
        ${COCOA_FRAMEWORK}
        ${COREGRAPHICS_FRAMEWORK}
    )
endif()

# Compiler flags
target_compile_options(SimpleSynthCLAP PRIVATE
    -Wall -Wextra -Wpedantic
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
)