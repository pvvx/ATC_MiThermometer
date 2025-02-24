TEL_CHIP := -DCHIP_TYPE=CHIP_TYPE_8258

LIBS := -llt_8258

TEL_PATH ?= ./SDK

PROJECT_NAME ?= ATC_Thermometer

PROJECT_PATH := ./src
OUT_PATH :=./out
ZBO_PATH :=./zigbee_ota

PGM_PORT?=COM9

ifneq ($(TEL_PATH)/components/drivers/8258/gpio_8258.c, $(wildcard $(TEL_PATH)/components/drivers/8258/gpio_8258.c))
$(error "Please check SDK Path and set TEL_PATH.")
endif

TL_Check = $(PROJECT_PATH)/../utils/tl_check_fw.py
zb_OTA = $(PROJECT_PATH)/../utils/zigbee_ota.py

COMPILEOS = $(shell uname -o)
LINUX_OS = GNU/Linux

ifeq ($(COMPILEOS),$(LINUX_OS))
	PYTHON ?= python3
	TOOLS_PATH := $(TEL_PATH)/tools/linux/
	TC32_PATH := $(TOOLS_PATH)tc32/bin/
else
	PYTHON ?= python
	TOOLS_PATH := $(TEL_PATH)/tools/windows/
ifeq ($(TOOLS_PATH)tc32/bin/tc32-elf-gcc.exe, $(wildcard $(TOOLS_PATH)tc32/bin/tc32-elf-gcc.exe))
	TC32_PATH := $(TOOLS_PATH)tc32/bin/
endif
endif

OBJ_SRCS := 
S_SRCS := 
ASM_SRCS := 
C_SRCS := 
S_UPPER_SRCS := 
O_SRCS := 
FLASH_IMAGE := 
ELFS := 
OBJS := 
LST := 
SIZEDUMMY := 
OUT_DIR :=

GCC_FLAGS := \
-ffunction-sections \
-fdata-sections \
-Wall \
-O2 \
-fpack-struct \
-fshort-enums \
-finline-small-functions \
-std=gnu99 \
-funsigned-char \
-fshort-wchar \
-fms-extensions

INCLUDE_PATHS := -I$(TEL_PATH)/components -I$(PROJECT_PATH)

GCC_FLAGS += $(TEL_CHIP)
GCC_FLAGS += $(POJECT_DEF)

LS_FLAGS := $(PROJECT_PATH)/boot.link

#include SDK makefile
#-include $(PROJECT_PATH)/make/application.mk
#-include $(PROJECT_PATH)/make/common.mk
#-include $(PROJECT_PATH)/make/vendor_common.mk
#-include $(PROJECT_PATH)/make/tinyFlash.mk
-include $(PROJECT_PATH)/uprintf.mk
-include $(PROJECT_PATH)/drivers_8258.mk
-include $(PROJECT_PATH)/div_mod.mk

ifeq ($(USE_FREE_RTOS), 1)
-include $(PROJECT_PATH)/freertos.mk
GCC_FLAGS += -DUSE_FREE_RTOS
endif

#include Project makefile
-include $(PROJECT_PATH)/project.mk
-include $(PROJECT_PATH)/boot.mk

# Add inputs and outputs from these tool invocations to the build variables 
LST_FILE := $(OUT_PATH)/$(PROJECT_NAME).lst
BIN_FILE := $(OUT_PATH)/../$(PROJECT_NAME).bin
ELF_FILE := $(OUT_PATH)/$(PROJECT_NAME).elf

SIZEDUMMY := sizedummy

# All Target
all: clean pre-build main-build

build: pre-build main-build


flash: $(BIN_FILE)
	@$(PYTHON) $(PROJECT_PATH)/../TlsrPgm.py -p$(PGM_PORT) -t150 -a2750 -m -w we 0 $(BIN_FILE)

# test restore from Zigbee
flash_zbr: $(BIN_FILE)
	@$(PYTHON) $(PROJECT_PATH)/../TlsrPgm.py -p$(PGM_PORT) -z11 -a-70 -s es 0 0x40000
	@$(PYTHON) $(PROJECT_PATH)/../TlsrPgm.py -p$(PGM_PORT) i
	@$(PYTHON) $(PROJECT_PATH)/../TlsrPgm.py -p$(PGM_PORT) -m we 0x40000 $(BIN_FILE)

erase:
	@$(PYTHON) $(PROJECT_PATH)/../TlsrPgm.py -p$(PGM_PORT) -t50 -a2750 -s ea

reset:
	@$(PYTHON) $(PROJECT_PATH)/../TlsrPgm.py -p$(PGM_PORT) -t50 -a2750 -m -w i

stop:
	@$(PYTHON) $(PROJECT_PATH)/../TlsrPgm.py -p$(PGM_PORT) -z11 -a-75 i

go:
	@$(PYTHON) $(PROJECT_PATH)/../TlsrPgm.py -p$(PGM_PORT) -w -m

TADDR?=0x844000
TLEN?=128
test_damp:
	@$(PYTHON) $(PROJECT_PATH)/../TlsrPgm.py -p$(PGM_PORT) -z11 -c -g ds $(TADDR) $(TLEN)

# Main-build Target
main-build: $(ELF_FILE) secondary-outputs

# Tool invocations
$(ELF_FILE): $(OBJS) $(USER_OBJS)
	@echo 'Building Standard target: $@'
	@$(TC32_PATH)tc32-elf-ld --gc-sections -L $(TEL_PATH)/components/proj_lib -L $(OUT_PATH) -T $(LS_FLAGS) -o $(ELF_FILE) $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Building Reduced target: $@'
	@$(TC32_PATH)tc32-elf-ld --gc-sections -Ttext `$(PYTHON) $(PROJECT_PATH)/TlsrRetMemAddr.py -e $(ELF_FILE) -t $(TC32_PATH)tc32-elf-nm` -L $(TEL_PATH)/components/proj_lib -L $(OUT_PATH) -T $(LS_FLAGS) -o $(ELF_FILE) $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

$(LST_FILE): $(ELF_FILE)
	@echo 'Invoking: TC32 Create Extended Listing'
	@$(TC32_PATH)tc32-elf-objdump -x -D -l -S  $(ELF_FILE)  > $(LST_FILE)
	@echo 'Finished building: $@'
	@echo ' '

$(BIN_FILE): $(ELF_FILE)
	@echo 'Create Flash image (binary format)'
	@$(TC32_PATH)tc32-elf-objcopy -v -O binary $(ELF_FILE)  $(BIN_FILE)
	@$(PYTHON) $(TL_Check) $(BIN_FILE)
	@echo 'Finished building: $@'
	@echo ' '
	@echo 'Create Zigbee OTA image'
	@mkdir -p $(ZBO_PATH)
	@$(PYTHON) $(zb_OTA) $(BIN_FILE) -p $(ZBO_PATH)
	@echo ' '

sizedummy: $(ELF_FILE)
	@$(PYTHON) $(PROJECT_PATH)/TlsrMemInfo.py -s 32768 -t $(TC32_PATH)tc32-elf-nm $(ELF_FILE)

clean:
	-$(RM) $(FLASH_IMAGE) $(ELFS) $(OBJS) $(LST) $(SIZEDUMMY) $(ELF_FILE) $(BIN_FILE) $(LST_FILE)
	-@echo 'Clean...  '

pre-build:
	mkdir -p $(foreach s,$(OUT_DIR),$(OUT_PATH)$(s))
	-@echo 'Create OUT-dir '
ifeq ($(COMPILEOS),$(LINUX_OS))
ifneq ($(TC32_PATH)tc32-elf-gcc, $(wildcard $(TC32_PATH)tc32-elf-gcc))
	@wget -P $(TOOLS_PATH) http://shyboy.oss-cn-shenzhen.aliyuncs.com/readonly/tc32_gcc_v2.0.tar.bz2 
	@tar -xvjf $(TOOLS_PATH)tc32_gcc_v2.0.tar.bz2 -C $(TOOLS_PATH)	
endif
endif

secondary-outputs: $(BIN_FILE) $(LST_FILE) $(SIZEDUMMY)

.PHONY: all clean
.SECONDARY: main-build