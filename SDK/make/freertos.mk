################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
OUT_DIR += /freertos /freertos/portable/MemMang /freertos/portable/gcc /freertos/portable/Common


OBJS += \
$(OUT_PATH)/freertos/croutine.o \
$(OUT_PATH)/freertos/event_groups.o \
$(OUT_PATH)/freertos/list.o \
$(OUT_PATH)/freertos/queue.o \
$(OUT_PATH)/freertos/stream_buffer.o \
$(OUT_PATH)/freertos/tasks.o \
$(OUT_PATH)/freertos/timers.o \
$(OUT_PATH)/freertos/portable/MemMang/heap_1.o \
$(OUT_PATH)/freertos/portable/gcc/port.o \
$(OUT_PATH)/freertos/portable/Common/mpu_wrappers.o

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/freertos/%.o: $(TEL_PATH)/components/freertos/%.c
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"