add_executable(testing
    main_testing.cpp
    ../src/SampleProcessor.cpp
)

add_dependencies(testing CMSISDSP)

target_link_libraries(testing
    pico_stdlib
    cmsis_core
    CMSISDSP
    FreeRTOS-Kernel-Heap4
    freertos_config

    DRIVERS
)
	
target_include_directories(testing PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../lib/include
    ${CMAKE_CURRENT_LIST_DIR}/../src

    ${CMAKE_CURRENT_LIST_DIR}/../config
    ${CMAKE_CURRENT_LIST_DIR}/../drivers
    ${CMAKE_CURRENT_LIST_DIR}/../lib/eigen3
)

# Add map/bin/hex file etc additionally.
pico_add_extra_outputs(testing)

pico_enable_stdio_usb(testing 1)
pico_enable_stdio_uart(testing 0)

# target_compile_definitions(${NAME} PRIVATE
#     PICO_DEFAULT_UART_RX_PIN=17
#     PICO_DEFAULT_UART_TX_PIN=16
# )
