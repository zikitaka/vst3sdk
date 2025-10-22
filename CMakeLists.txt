cmake_minimum_required(VERSION 3.15)
project(OBSEQ_VST3 VERSION 1.0.0)

# Path to the Steinberg SDK
set(VST3_SDK_DIR "${CMAKE_SOURCE_DIR}/vst3sdk")

# Include the SDK’s CMake helpers
include("${VST3_SDK_DIR}/cmake/modules/SMTG_VST3_SDK.cmake")

# Set up platform toolset
smtg_setup_platform_toolset()

# Your plugin source file
add_library(OBSEQ_VST3 MODULE
    plugin.cpp
)

# Include SDK headers
target_include_directories(OBSEQ_VST3 PRIVATE
    ${VST3_SDK_DIR}
)

# Register as a VST3 plugin
smtg_add_vst3plugin(OBSEQ_VST3)
