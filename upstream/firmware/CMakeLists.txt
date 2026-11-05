# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Wayfarer C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(Wayfarer Wayfarer.c game.c generated_events.c ui.c docking.c crew.c asteroids.c nebulagame.c multiplayer.c frontier.c mazegame.c contracts.c frontier_img.c minigames.c touch.c)

target_include_directories(Wayfarer PUBLIC include)
target_include_directories(Wayfarer PUBLIC bsp)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/bsp)

target_link_libraries(Wayfarer bsp)

pico_set_program_name(Wayfarer "Wayfarer")
pico_set_program_version(Wayfarer "0.2-frontier")

# Generate PIO header
pico_generate_pio_header(Wayfarer ${CMAKE_CURRENT_LIST_DIR}/blink.pio)

target_compile_definitions(Wayfarer PRIVATE 
    PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64
)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Wayfarer 0)
pico_enable_stdio_usb(Wayfarer 1)

# Add the standard library to the build
target_link_libraries(Wayfarer
        pico_stdlib
        pico_rand)

# Add the standard include files to the build
target_include_directories(Wayfarer PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/bsp
)

# Add any user requested libraries
target_link_libraries(Wayfarer 
        hardware_i2c
        hardware_pio
        bsp
        )

pico_add_extra_outputs(Wayfarer)

