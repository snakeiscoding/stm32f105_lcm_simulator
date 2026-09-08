cmake_minimum_required(VERSION 3.20)
project(white_simulator C ASM)

set(CMAKE_TOOLCHAIN_FILE cmake/arm-gcc-toolchain.cmake)

# build the bootloader
add_subdirectory(BootAPI)

# define the simulator executable
add_executable(white_simulator
    Source/ADC.c
    Source/ADS1100.c
    Source/EE93CXX.c
    Source/Encoder.c
    Source/JHTLCBComm.c
    Source/LCBSimulator.c
    Source/LCM.c
    Source/LCMDisplay.c
    Source/Operation.c
    Source/RowerWatts.c
    Source/Screen.c
    Source/SimulatorMain.c
    Source/Sintable.c
    Source/stm32f10x_it.c
    Source/system_stm32f10x.c
    Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_cl.s
    Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c
    Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.c
    Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c
    Libraries/STM32F10x_StdPeriph_Driver/src/misc.c
    Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_tim.c
    Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_adc.c
    Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_dma.c
)

target_include_directories(white_simulator PUBLIC Inc)
target_include_directories(white_simulator PUBLIC Libraries/STM32F10x_StdPeriph_Driver/inc)
target_include_directories(white_simulator PUBLIC Libraries/CMSIS/CM3/CoreSupport)
target_include_directories(white_simulator PUBLIC Libraries/CMSIS/CM3/CoreSupport)
target_include_directories(white_simulator PUBLIC BootAPI/inc)

target_compile_options(white_simulator PRIVATE
    -mcpu=cortex-m3
    -mthumb
    -std=c11
    -O1
    -ffunction-sections
    -fdata-sections
    -fno-common
)

target_link_options(white_simulator PRIVATE
    "-T${CMAKE_CURRENT_SOURCE_DIR}/white_simulator.ld"
    -mcpu=cortex-m3
    -mthumb
    -Wl,--gc-sections
    -nostartfiles
    -Wl,-Map=firmware.map
)

set_target_properties(white_simulator PROPERTIES
    OUTPUT_NAME "white_simulator"
    SUFFIX ".elf"
)

add_custom_command(TARGET white_simulator POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary
            $<TARGET_FILE:white_simulator>
            white_simulator.bin
)

add_custom_command(TARGET white_simulator POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex
            $<TARGET_FILE:white_simulator>
            white_simulator.hex
)

# this command combines the bootloader and application into a single binary file for flashing
add_custom_command(
    OUTPUT white_simulator_with_bootloader.bin
    COMMAND srec_cat
        ${CMAKE_SOURCE_DIR}/build/BootAPI/white_simulator_bootloader.hex
            -Intel -crop 0x08000000 0x0800D000 -offset -0x08000000
        ${CMAKE_SOURCE_DIR}/build/white_simulator.hex
            -Intel -crop 0x0800D000 0x08040000 -offset -0x08000000
        -o white_simulator_with_bootloader.bin -Binary
    DEPENDS
        white_simulator
        white_simulator_bootloader
    COMMENT "Combining bootloader and application into combined binary file"
)
add_custom_target(white_simulator_combined ALL
    DEPENDS white_simulator_with_bootloader.bin
)
