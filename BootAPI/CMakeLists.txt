### BOOTLOADER

add_executable(white_simulator_bootloader
    src/Boot_it.c
    src/Boot.c
    src/Bootflash.c
    src/BootLCM.c
    src/Stm32GPIO.c
    LowLevelInitial/manual_init.c
    ../Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_cl.s
    ../Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c
    ../Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.c
    ../Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c
    ../Libraries/STM32F10x_StdPeriph_Driver/src/misc.c
    ../Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_tim.c
    ../Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_adc.c
    ../Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_dma.c
    ../Libraries/FileSystem/fat/src/mst/mst.c
    ../Libraries/FileSystem/fat/src/common/common.c
    ../Libraries/FileSystem/fat/src/common/fat.c
    ../Libraries/FileSystem/fat/src/common/fat_lfn.c
    ../Libraries/FileSystem/fat/src/common/fat_m.c
    ../Libraries/FileSystem/fat/src/common/port_f.c
    ../Libraries/FileSystem/usb_host/usb_driver/mst/scsi.c
    ../Libraries/FileSystem/usb_host/usb_driver/mst/usb_mst.c
    ../Libraries/FileSystem/usb_host/pmgr/pmgr.c
    ../Libraries/FileSystem/usb_host/stm32uh/stm32uh.c
    ../Libraries/FileSystem/usb_host/stm32uh/stm32uh_hub.c
    ../Libraries/FileSystem/usb_host/stm32uh/stm32uh_hw.c
    ../Libraries/FileSystem/usb_host/usb_host.c
    ../Libraries/FileSystem/usb_host/usb_utils.c
    ../Libraries/FileSystem/os/nrtos/os_nrtos.c
)

target_include_directories(white_simulator_bootloader PUBLIC ${CMAKE_SOURCE_DIR}/Inc)
target_include_directories(white_simulator_bootloader PUBLIC ${CMAKE_SOURCE_DIR}/BootAPI/inc)
target_include_directories(white_simulator_bootloader PUBLIC ${CMAKE_SOURCE_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc)
target_include_directories(white_simulator_bootloader PUBLIC ${CMAKE_SOURCE_DIR}/Libraries/CMSIS/CM3/CoreSupport)
target_include_directories(white_simulator_bootloader PUBLIC ${CMAKE_SOURCE_DIR}/Libraries/CMSIS/CM3/CoreSupport)
target_include_directories(white_simulator_bootloader PUBLIC ${CMAKE_SOURCE_DIR}/Libraries/FileSystem)
target_include_directories(white_simulator_bootloader PUBLIC ${CMAKE_SOURCE_DIR}/Libraries/FileSystem/os)
target_include_directories(white_simulator_bootloader PUBLIC ${CMAKE_SOURCE_DIR}/Libraries/FileSystem/os/nrtos)
target_include_directories(white_simulator_bootloader PUBLIC ${CMAKE_SOURCE_DIR}/Libraries/FileSystem/fat/src/common)
target_include_directories(white_simulator_bootloader PUBLIC ${CMAKE_SOURCE_DIR}/Libraries/FileSystem/fat/src/mst)
target_include_directories(white_simulator_bootloader PUBLIC ${CMAKE_SOURCE_DIR}/Libraries/FileSystem/usb_host/usb_driver/mst)
target_include_directories(white_simulator_bootloader PUBLIC ${CMAKE_SOURCE_DIR}/Libraries/FileSystem/usb_host)
target_include_directories(white_simulator_bootloader PUBLIC ${CMAKE_SOURCE_DIR}/Libraries/FileSystem/usb_host/pmgr)
target_include_directories(white_simulator_bootloader PUBLIC ${CMAKE_SOURCE_DIR}/Libraries/FileSystem/usb_host/stm32uh)

target_compile_options(white_simulator_bootloader PRIVATE
    -mcpu=cortex-m3
    -mthumb
    -std=c11
    -O1
    -ffunction-sections
    -fdata-sections
    -fno-common
)

target_link_options(white_simulator_bootloader PRIVATE
    "-T${CMAKE_CURRENT_SOURCE_DIR}/white_simulator_bootloader.ld"
    -mcpu=cortex-m3
    -mthumb
    -Wl,--gc-sections
    -nostartfiles
    -Wl,-Map=firmware.map
)

set_target_properties(white_simulator_bootloader PROPERTIES
    OUTPUT_NAME "white_simulator_bootloader"
    SUFFIX ".elf"
)

add_custom_command(TARGET white_simulator_bootloader POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary
            $<TARGET_FILE:white_simulator_bootloader>
            white_simulator_bootloader.bin
)

add_custom_command(TARGET white_simulator_bootloader POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex
            $<TARGET_FILE:white_simulator_bootloader>
            white_simulator_bootloader.hex
)