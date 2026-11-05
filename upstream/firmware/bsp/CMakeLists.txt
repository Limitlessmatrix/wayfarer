set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/*.c")
file(GLOB DIR_BSP_SRCS "${CMAKE_CURRENT_LIST_DIR}/*.c")

add_library(bsp ${DIR_BSP_SRCS})


# Add the standard include files to the build
target_include_directories(bsp PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
)

pico_generate_pio_header(bsp ${CMAKE_CURRENT_LIST_DIR}/pio_qspi.pio)
pico_generate_pio_header(bsp ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)

target_link_libraries(bsp PUBLIC 
    pico_stdlib 
    pico_stdio_usb
    pico_rand
    hardware_spi 
    hardware_i2c 
    hardware_pwm 
    hardware_dma
    hardware_pio
    hardware_irq
    hardware_timer
    pico_unique_id)



