
OUT_DIR += /src

OBJS += \
$(OUT_PATH)/src/utils.o \
$(OUT_PATH)/src/app.o \
$(OUT_PATH)/src/lcd.o \
$(OUT_PATH)/src/lcd_lywsd03mmc.o \
$(OUT_PATH)/src/lcd_cgdk2.o \
$(OUT_PATH)/src/lcd_mjwsd05mmc.o \
$(OUT_PATH)/src/lcd_mho_c122.o \
$(OUT_PATH)/src/lcd_th03.o \
$(OUT_PATH)/src/lcd_lktmzl02.o \
$(OUT_PATH)/src/lcd_zth05z.o \
$(OUT_PATH)/src/epd_cgg1.o \
$(OUT_PATH)/src/epd_cgg1n.o \
$(OUT_PATH)/src/epd_mho_c401.o \
$(OUT_PATH)/src/epd_mho_c401n.o \
$(OUT_PATH)/src/sensors.o \
$(OUT_PATH)/src/ina226.o \
$(OUT_PATH)/src/my18b20.o \
$(OUT_PATH)/src/hx71x.o \
$(OUT_PATH)/src/rtc_pcf85163.o \
$(OUT_PATH)/src/trigger.o \
$(OUT_PATH)/src/rds_count.o \
$(OUT_PATH)/src/app_att.o \
$(OUT_PATH)/src/battery.o \
$(OUT_PATH)/src/rh.o \
$(OUT_PATH)/src/ble.o \
$(OUT_PATH)/src/i2c.o \
$(OUT_PATH)/src/cmd_parser.o \
$(OUT_PATH)/src/ext_ota.o \
$(OUT_PATH)/src/flash_eep.o \
$(OUT_PATH)/src/logger.o \
$(OUT_PATH)/src/blt_common.o\
$(OUT_PATH)/src/ccm.o \
$(OUT_PATH)/src/custom_beacon.o \
$(OUT_PATH)/src/mi_beacon.o \
$(OUT_PATH)/src/bthome_beacon.o \
$(OUT_PATH)/src/main.o


# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/src/%.o: $(PROJECT_PATH)/%.c
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"