cmake_minimum_required(VERSION 3.15)
project(ThreeBandEQ VERSION 1.0.0)

set(VST3_SDK_DIR "${CMAKE_SOURCE_DIR}/vst3sdk")

# Include Steinberg helpers
include("${VST3_SDK_DIR}/cmake/modules/SMTG_VST3_SDK.cmake")
smtg_setup_platform_toolset()

add_library(ThreeBandEQ MODULE plugin.cpp)
smtg_target_add_vst3_category(ThreeBandEQ "Fx")
smtg_target_set_version(ThreeBandEQ ${PROJECT_VERSION})
set_target_properties(ThreeBandEQ PROPERTIES SUFFIX ".vst3")
