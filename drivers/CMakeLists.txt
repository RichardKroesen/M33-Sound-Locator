add_library(DRIVERS INTERFACE)

target_include_directories(DRIVERS INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(DRIVERS INTERFACE
    pico_stdlib
    hardware_adc
    hardware_pwm
    hardware_dma

    FreeRTOS-Kernel-Heap4
    freertos_config
)