# =============================================================================
#  STM32F105 LCB Simulator — Linux build
#
#  唯一需求：arm-none-eabi-gcc + binutils（不需要 CMake / Ninja / SRecord / IAR）
#      Ubuntu 20.04+  sudo apt install gcc-arm-none-eabi libnewlib-arm-none-eabi
#      Ubuntu 14.04   見 README-linux-build.md
#
#      make            編出 bootloader + app + 合併燒錄檔（輸出在 out/）
#      make -j4        平行編譯
#      make clean
#      make compdb     產生 compile_commands.json（clangd / VS Code，選用）
#
#  首次使用前請先執行一次 `sh fix-case.sh`，修正原始碼中 9 處 #include 的大小寫
#  （Windows 檔案系統不分大小寫，這些錯誤在 Linux 上會直接編不過）。
#
#  與既有的 CMake 建置各自獨立、可並存：Makefile -> out/、CMake -> build/
# =============================================================================

# 工具鏈前綴。優先序：make CROSS=... > toolchain.mk > 環境變數 > PATH 上的第一個。
# 建議在 toolchain.mk 裡寫死完整路徑，這樣建置結果不受 $PATH 影響，例如：
#     CROSS = /opt/toolchains/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-
-include toolchain.mk
CROSS   ?= arm-none-eabi-
CC      := $(CROSS)gcc
OBJCOPY := $(CROSS)objcopy
SIZE    := $(CROSS)size
BUILD   ?= out

CPU     := -mcpu=cortex-m3 -mthumb
CFLAGS  := $(CPU) -std=c11 -O1 -ffunction-sections -fdata-sections -fno-common
LDFLAGS := $(CPU) -Wl,--gc-sections -nostartfiles
#          ^^^ -O1 不可更動：其他最佳化等級韌體不會正常運作（見 README.md）

# 額外旗標掛勾，供 toolchain.mk 或命令列追加而不必改上面兩行。
# 舊工具鏈（GCC 10.x）連結 bootloader 時會多拉進 newlib 的 abort/_exit/_kill，
# 撐爆 FLASH 尾端，需要：EXTRA_LDFLAGS = --specs=nano.specs --specs=nosys.specs
CFLAGS  += $(EXTRA_CFLAGS)
LDFLAGS += $(EXTRA_LDFLAGS)

# Flash 配置，須與兩份 linker script 一致
BOOT_ORIGIN := 0x08000000
APP_ORIGIN  := 0x0800D000
BOOT_SIZE   := $(shell printf '%d' $$(( $(APP_ORIGIN) - $(BOOT_ORIGIN) )))

STARTUP   := Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_cl.s
STDPERIPH := $(addprefix Libraries/STM32F10x_StdPeriph_Driver/src/, \
                 stm32f10x_gpio.c stm32f10x_usart.c stm32f10x_rcc.c misc.c \
                 stm32f10x_tim.c stm32f10x_adc.c stm32f10x_dma.c)

# ─── 應用程式 ────────────────────────────────────────────────────────────────
APP     := white_simulator
APP_LD  := white_simulator.ld
APP_INC := Inc BootAPI/inc \
           Libraries/CMSIS/CM3/CoreSupport \
           Libraries/STM32F10x_StdPeriph_Driver/inc
APP_SRC := $(addprefix Source/, \
               ADC.c ADS1100.c EE93CXX.c Encoder.c JHTLCBComm.c LCBSimulator.c \
               LCM.c LCMDisplay.c Operation.c RowerWatts.c Screen.c \
               SimulatorMain.c Sintable.c stm32f10x_it.c system_stm32f10x.c) \
           $(STARTUP) $(STDPERIPH)

# ─── Bootloader ──────────────────────────────────────────────────────────────
BOOT     := white_simulator_bootloader
BOOT_LD  := BootAPI/white_simulator_bootloader.ld
BOOT_INC := Inc BootAPI/inc \
            Libraries/CMSIS/CM3/CoreSupport \
            Libraries/STM32F10x_StdPeriph_Driver/inc \
            Libraries/FileSystem \
            Libraries/FileSystem/os Libraries/FileSystem/os/nrtos \
            Libraries/FileSystem/fat/src/common Libraries/FileSystem/fat/src/mst \
            Libraries/FileSystem/usb_host Libraries/FileSystem/usb_host/pmgr \
            Libraries/FileSystem/usb_host/stm32uh \
            Libraries/FileSystem/usb_host/usb_driver/mst
BOOT_SRC := $(addprefix BootAPI/src/, \
                Boot_it.c Boot.c Bootflash.c BootLCM.c Stm32GPIO.c) \
            BootAPI/LowLevelInitial/manual_init.c \
            $(STARTUP) $(STDPERIPH) \
            $(addprefix Libraries/FileSystem/, \
                fat/src/mst/mst.c fat/src/common/common.c fat/src/common/fat.c \
                fat/src/common/fat_lfn.c fat/src/common/fat_m.c fat/src/common/port_f.c \
                usb_host/usb_driver/mst/scsi.c usb_host/usb_driver/mst/usb_mst.c \
                usb_host/pmgr/pmgr.c usb_host/stm32uh/stm32uh.c \
                usb_host/stm32uh/stm32uh_hub.c usb_host/stm32uh/stm32uh_hw.c \
                usb_host/usb_host.c usb_host/usb_utils.c os/nrtos/os_nrtos.c)

# ─── 規則 ────────────────────────────────────────────────────────────────────
# ※ 上面兩份來源清單刻意逐一列舉、且順序與原 CMakeLists.txt 一致（不是字母序）。
#   來源順序即連結順序，會決定 section 擺放；順序一改，產出的 binary 就不再與
#   CMake / Windows 版位元組一致（功能仍等價，但失去交叉驗證的能力）。
#   基於同樣理由不要改用 $(wildcard)。

objs     = $(addprefix $(BUILD)/$1/,$(addsuffix .o,$(basename $2)))
APP_OBJ  := $(call objs,$(APP),$(APP_SRC))
BOOT_OBJ := $(call objs,$(BOOT),$(BOOT_SRC))

.PHONY: all clean compdb
.SECONDARY:

all: $(BUILD)/$(APP)_with_bootloader.bin $(BUILD)/$(APP).hex $(BUILD)/$(BOOT).hex
	@$(SIZE) $(BUILD)/$(APP).elf $(BUILD)/$(BOOT).elf
	@{ command -v $(CC); $(CC) -dumpversion; } | paste -sd' ' > $(BUILD)/toolchain.txt
	@echo "built by: $$(cat $(BUILD)/toolchain.txt)"

# app 與 bootloader 共用 8 個來源檔但 include path 不同，因此各自一棵物件樹
$(APP_OBJ):  INCFLAGS := $(addprefix -I,$(APP_INC))
$(BOOT_OBJ): INCFLAGS := $(addprefix -I,$(BOOT_INC))

define compile_rules
$(BUILD)/$1/%.o: %.c
	@mkdir -p $$(@D)
	$$(CC) $$(CFLAGS) $$(INCFLAGS) -MMD -MP -c $$< -o $$@
$(BUILD)/$1/%.o: %.s
	@mkdir -p $$(@D)
	$$(CC) $$(CFLAGS) $$(INCFLAGS) -MMD -MP -c $$< -o $$@
endef
$(eval $(call compile_rules,$(APP)))
$(eval $(call compile_rules,$(BOOT)))

$(BUILD)/$(APP).elf: $(APP_OBJ) $(APP_LD)
	$(CC) $(APP_OBJ) $(LDFLAGS) -T$(APP_LD) -Wl,-Map=$(@:.elf=.map) -o $@

$(BUILD)/$(BOOT).elf: $(BOOT_OBJ) $(BOOT_LD)
	$(CC) $(BOOT_OBJ) $(LDFLAGS) -T$(BOOT_LD) -Wl,-Map=$(@:.elf=.map) -o $@

$(BUILD)/%.bin: $(BUILD)/%.elf
	$(OBJCOPY) -O binary $< $@

$(BUILD)/%.hex: $(BUILD)/%.elf
	$(OBJCOPY) -O ihex $< $@

# 取代 srec_cat：bootloader 裁切到 APP_ORIGIN，其後直接接上 app
$(BUILD)/$(APP)_with_bootloader.bin: $(BUILD)/$(BOOT).bin $(BUILD)/$(APP).bin
	head -c $(BOOT_SIZE) $(BUILD)/$(BOOT).bin > $@
	cat $(BUILD)/$(APP).bin >> $@

clean:
	rm -rf $(BUILD) compile_commands.json

compdb:
	@{ echo '['; \
	   $(foreach t,APP BOOT, \
	     for f in $($(t)_SRC); do \
	       printf '  {"directory":"$(CURDIR)","file":"%s","command":"$(CC) $(CFLAGS) $(addprefix -I,$($(t)_INC)) -c %s"},\n' "$$f" "$$f"; \
	     done;) \
	 } | sed '$$ s/,$$//' > $@.tmp
	@{ cat $@.tmp; echo ']'; } > compile_commands.json && rm -f $@.tmp
	@echo "compile_commands.json: $$(grep -c '\"file\"' compile_commands.json) 筆"

-include $(APP_OBJ:.o=.d) $(BOOT_OBJ:.o=.d)
