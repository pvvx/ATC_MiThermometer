################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 

OUT_DIR += /vendor/common

OBJS += \
$(OUT_PATH)/vendor/common/blt_common.o \
$(OUT_PATH)/vendor/common/blt_fw_sign.o \
$(OUT_PATH)/vendor/common/blt_led.o \
$(OUT_PATH)/vendor/common/blt_soft_timer.o \
$(OUT_PATH)/vendor/common/tl_audio.o 

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/vendor/common/%.o: $(TEL_PATH)/components/vendor/common/%.c $(common_dir)
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"

