
OUT_DIR += /src

OBJS += $(OUT_PATH)/src/div_mod.o

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/src/%.o: $(PROJECT_PATH)/%.S
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(BOOT_FLAG) -c -o"$@" "$<"
