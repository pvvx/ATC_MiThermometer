################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
OUT_DIR += /common


OBJS += \
$(OUT_PATH)/common/breakpoint.o \
$(OUT_PATH)/common/log.o \
$(OUT_PATH)/common/selection_sort.o \
$(OUT_PATH)/common/string.o \
$(OUT_PATH)/common/utility.o 


# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/common/%.o: $(TEL_PATH)/components/common/%.c
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"