# Generated CMake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Determine user home directory
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

# SDK, Toolchain, and Picotool versions
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)

# Include VSCode configuration if available
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()

# Set PICO_SDK_PATH
set(PICO_SDK_PATH "/home/helen/.pico-sdk/sdk/2.1.0")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)
include_directories(${PICO_SDK_PATH}/src)

# Configure the project
project(animacoes_led C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add the main executable
add_executable(animacoes_led 
    animacoes_led.c         # Código principal do projeto
    ${CMAKE_CURRENT_LIST_DIR}/pio_matrix.pio
)

# Set program name and version
pico_set_program_name(animacoes_led "animacoes_led")
pico_set_program_version(animacoes_led "0.1")

# Enable/disable output over UART/USB
pico_enable_stdio_uart(animacoes_led 0)
pico_enable_stdio_usb(animacoes_led 1)

# Generate the PIO header
pico_generate_pio_header(animacoes_led ${CMAKE_CURRENT_LIST_DIR}/pio_matrix.pio)

# Add the standard library and required hardware libraries
target_link_libraries(animacoes_led 
    pico_stdlib       # Biblioteca padrão do Pico
    hardware_pio      # Biblioteca para manipulação de PIO
    hardware_clocks   # Biblioteca para controle de clocks
    hardware_adc      # Biblioteca para ADC (caso esteja sendo usada)
)

# Add the standard include files to the build
target_include_directories(animacoes_led PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Add extra outputs (e.g., UF2 files)
pico_add_extra_outputs(animacoes_led)
