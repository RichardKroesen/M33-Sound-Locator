cmake_minimum_required(VERSION 3.18)

set(PICO_BOARD pico2)
set(PICO_PLATFORM rp2350)
set(PICO_SDK_FETCH_FROM_GIT ON)

# set(picotool_DIR "C:/1_DEV/picotool")
# message(STATUS "Using picotool directory: ${picotool_DIR}")

set(NAME LINEAR_SYSTEMS)

include(FetchContent)
include(pico_sdk_import.cmake)

# Compiler options
project(${NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_CXX_ENABLE_EXCEPTIONS 0) # Disable exceptions for now...

# Initialize the SDK
pico_sdk_init()

#include Libraries
set(FREERTOS_CONFIG_FILE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/config" CACHE STRING "Local Config")
set(FREERTOS_KERNEL_PATH "${CMAKE_CURRENT_LIST_DIR}/lib" CACHE STRING "FreeRTOS Kernel Path")
include("${FREERTOS_CONFIG_FILE_DIRECTORY}/freertos_config.cmake") 
include(FreeRTOS_Kernel_import.cmake)

add_compile_options(-Wall -Wno-format -Wno-unused-function -Wno-maybe-uninitialized)

# Link to libraries and set target executable
add_subdirectory(src)
