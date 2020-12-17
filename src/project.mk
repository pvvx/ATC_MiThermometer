OBJS += \
$(OUT_PATH)/app.o \
$(OUT_PATH)/app_att.o \
$(OUT_PATH)/battery.o \
$(OUT_PATH)/ble.o \
$(OUT_PATH)/i2c.o \
$(OUT_PATH)/lcd.o \
$(OUT_PATH)/sensor.o \
$(OUT_PATH)/cmd_parser.o \
$(OUT_PATH)/flash.o \
$(OUT_PATH)/main.o


# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/%.o: ./src/%.c
	@echo 'Building file: $<'
	@tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"