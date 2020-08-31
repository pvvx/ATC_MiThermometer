TEL_CHIP := -DCHIP_TYPE=CHIP_TYPE_8258

LIBS :=  -llt_8258

TEL_PATH := ../..

PROJECT_NAME := ATC_Thermometer

PROJECT_PATH := .
OUT_PATH :=$(PROJECT_PATH)/out

ifneq ($(TEL_PATH)/make/makefile, $(wildcard $(TEL_PATH)/make/makefile))
$(error "Please check SDK_Path")
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
-fshort-wchar \
-fms-extensions

INCLUDE_PATHS := -I$(TEL_PATH)/components -I$(PROJECT_PATH)

GCC_FLAGS += $(TEL_CHIP)

#ifeq ($(RETENTION_RAM_SIZE), 32KB)
	LS_FLAGS := $(TEL_PATH)/components/boot/boot_32k_retn_8253_8258.link
#else
#	LS_FLAGS := $(TEL_PATH)/components/boot/boot_16k_retn_8251_8253_8258.link
#endif


#include SDK makefile
-include $(TEL_PATH)/make/application.mk
-include $(TEL_PATH)/make/boot.mk
-include $(TEL_PATH)/make/common.mk
-include $(TEL_PATH)/make/drivers_8258.mk
-include $(TEL_PATH)/make/vendor_common.mk
-include $(TEL_PATH)/make/tinyFlash.mk

ifeq ($(USE_FREE_RTOS), 1)
-include $(TEL_PATH)/make/freertos.mk
GCC_FLAGS += -DUSE_FREE_RTOS
endif

#include Project makefile
-include $(PROJECT_PATH)/project.mk

# Add inputs and outputs from these tool invocations to the build variables 
LST_FILE := $(OUT_PATH)/$(PROJECT_NAME).lst
BIN_FILE := $(OUT_PATH)/../$(PROJECT_NAME).bin
ELF_FILE := $(OUT_PATH)/$(PROJECT_NAME).elf

SIZEDUMMY := sizedummy

# All Target
all: clean pre-build main-build

# Main-build Target
main-build: $(ELF_FILE) secondary-outputs

# Tool invocations
$(ELF_FILE): $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@tc32-elf-ld --gc-sections -L $(TEL_PATH)/components/proj_lib -T $(LS_FLAGS) -o $(ELF_FILE) $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

$(LST_FILE): $(ELF_FILE)
	@echo 'Invoking: TC32 Create Extended Listing'
	tc32-elf-objdump -x -D -l -S  $(ELF_FILE)  > $(LST_FILE)
	@echo 'Finished building: $@'
	@echo ' '

$(BIN_FILE): $(ELF_FILE)
	@echo 'Create Flash image (binary format)'
	tc32-elf-objcopy -v -O binary $(ELF_FILE)  $(BIN_FILE)
	python3 $(TEL_PATH)/make/tl_firmware_tools.py add_crc $(BIN_FILE)
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: $(ELF_FILE)
	@echo 'Invoking: Print Size'
	tc32-elf-size -t $(ELF_FILE)
	@echo 'Finished building: $@'
	@echo ' '

clean:
	-$(RM) $(FLASH_IMAGE) $(ELFS) $(OBJS) $(LST) $(SIZEDUMMY) $(ELF_FILE) $(BIN_FILE) $(LST_FILE)
	-@echo ' '

pre-build:
	mkdir -p $(foreach s,$(OUT_DIR),$(OUT_PATH)$(s))
	-@echo ' '

secondary-outputs: $(BIN_FILE) $(LST_FILE) $(SIZEDUMMY)

.PHONY: all clean
.SECONDARY: main-build 