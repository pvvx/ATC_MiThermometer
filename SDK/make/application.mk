################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 

OUT_DIR += /application/app /application/keyboard /application/print /application/usbstd /application/TLV_Flash

OBJS += \
$(OUT_PATH)/application/app/usbaud.o \
$(OUT_PATH)/application/app/usbcdc.o \
$(OUT_PATH)/application/app/usbkb.o \
$(OUT_PATH)/application/app/usbmouse.o \
$(OUT_PATH)/application/keyboard/keyboard.o \
$(OUT_PATH)/application/print/putchar.o \
$(OUT_PATH)/application/print/u_printf.o \
$(OUT_PATH)/application/usbstd/usb.o \
$(OUT_PATH)/application/usbstd/usbdesc.o \
$(OUT_PATH)/application/usbstd/usbhw.o   

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/application/%.o: $(TEL_PATH)/components/application/%.c 
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"
