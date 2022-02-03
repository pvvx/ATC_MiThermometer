################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 

OUT_DIR += /drivers/8258

OBJS += \
$(OUT_PATH)/drivers/8258/adc.o \
$(OUT_PATH)/drivers/8258/aes.o \
$(OUT_PATH)/drivers/8258/analog.o \
$(OUT_PATH)/drivers/8258/audio.o \
$(OUT_PATH)/drivers/8258/bsp.o \
$(OUT_PATH)/drivers/8258/clock.o \
$(OUT_PATH)/drivers/8258/emi.o \
$(OUT_PATH)/drivers/8258/flash.o \
$(OUT_PATH)/drivers/8258/gpio_8258.o \
$(OUT_PATH)/drivers/8258/i2c.o \
$(OUT_PATH)/drivers/8258/lpc.o \
$(OUT_PATH)/drivers/8258/qdec.o \
$(OUT_PATH)/drivers/8258/rf_pa.o \
$(OUT_PATH)/drivers/8258/s7816.o \
$(OUT_PATH)/drivers/8258/spi.o \
$(OUT_PATH)/drivers/8258/timer.o \
$(OUT_PATH)/drivers/8258/uart.o \
$(OUT_PATH)/drivers/8258/watchdog.o 

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/drivers/8258/%.o: $(TEL_PATH)/components/drivers/8258/%.c
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"
