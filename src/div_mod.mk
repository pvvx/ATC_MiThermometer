
OUT_DIR += /drivers

OBJS += $(OUT_PATH)/drivers/div_mod.o

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/drivers/%.o: $(TEL_PATH)/components/boot/%.S
	@echo 'Building file: $<'
	@tc32-elf-gcc $(BOOT_FLAG) -c -o"$@" "$<"
