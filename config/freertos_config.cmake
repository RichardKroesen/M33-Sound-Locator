cmake_minimum_required(VERSION 3.15)

add_library(freertos_config STATIC)

target_sources(freertos_config PUBLIC   
        ${CMAKE_CURRENT_LIST_DIR}/IdleMemory.c
)

target_include_directories(freertos_config PUBLIC
	${CMAKE_CURRENT_LIST_DIR}
) 

target_compile_definitions(freertos_config
	PUBLIC 
)

target_link_libraries(freertos_config
	FreeRTOS-Kernel
	pico_stdlib
)