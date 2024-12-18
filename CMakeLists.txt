cmake_minimum_required(VERSION 3.18)

set(PICO_BOARD pico2)
set(PICO_PLATFORM rp2350)
set(NAME LINEAR_SYSTEMS)

set(PICO_SDK_FETCH_FROM_GIT ON)
set(PICOTOOL_FETCH_FROM_GIT_PATH ON)
message("Pico SDK Source Directory: ${pico_sdk_SOURCE_DIR}")
if (NOT DEFINED PICO_SDK_PATH)
    set(PICO_SDK_PATH "$ENV{PICO_SDK_PATH}")
endif()

include(FetchContent)
include(pico_sdk_import.cmake)

# Compiler options
project(${NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_CXX_ENABLE_EXCEPTIONS 0) 
set(CMSISDSP "${CMAKE_CURRENT_LIST_DIR}/lib/CMSIS-DSP")

# Initialize the SDK
pico_sdk_init()

#include Libraries
set(FREERTOS_CONFIG_FILE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/config" CACHE STRING "Local Config")
set(FREERTOS_KERNEL_PATH "${CMAKE_CURRENT_LIST_DIR}/lib/FreeRTOS-Kernel" CACHE STRING "FreeRTOS Kernel Path")
include("${FREERTOS_CONFIG_FILE_DIRECTORY}/freertos_config.cmake") 
include(FreeRTOS_Kernel_import.cmake)
include_directories(${pico_sdk_SOURCE_DIR}/src/rp2_common/cmsis/stub/CMSIS/Core/Include)

add_compile_options(-Wall -Wno-format -Wno-unused-function -Wno-maybe-uninitialized
-Ofast)

# Link to libraries and set target executable
add_subdirectory(src)
add_subdirectory(${CMSISDSP}/Source bin_dsp)