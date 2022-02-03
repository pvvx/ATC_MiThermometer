################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
OUT_DIR += /tinyFlash


OBJS += $(OUT_PATH)/tinyFlash/tinyFlash.o 


# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/tinyFlash/%.o: $(TEL_PATH)/components/tinyFlash/%.c
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"