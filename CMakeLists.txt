cmake_minimum_required(VERSION 3.18)

set(PICO_BOARD pico2)
set(PICO_PLATFORM rp2350)
set(PICO_SDK_FETCH_FROM_GIT ON)
set(NAME LINEAR_SYSTEMS)

# include(FetchContent) - Not used yet.
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
include_directories("${FREERTOS_CONFIG_FILE_DIRECTORY}") 
set(FREERTOS_KERNEL_PATH "${CMAKE_CURRENT_LIST_DIR}/lib/FreeRTOS-Kernel" CACHE STRING "FreeRTOS Kernel Path")
include(FreeRTOS_Kernel_import.cmake)

add_compile_options(-Wall -Wno-format -Wno-unused-function -Wno-maybe-uninitialized)

# Link to libraries and set target executable
add_subdirectory(src)