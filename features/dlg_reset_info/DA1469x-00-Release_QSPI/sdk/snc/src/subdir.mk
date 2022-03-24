################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src/SeNIS.c \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src/snc_debug.c \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src/snc_defs.c \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src/snc_emu.c \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src/snc_hw_gpadc.c \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src/snc_hw_gpio.c \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src/snc_hw_i2c.c \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src/snc_hw_spi.c \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src/snc_hw_sys.c \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src/snc_hw_uart.c \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src/snc_main.c \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src/snc_queues.c 

OBJS += \
./sdk/snc/src/SeNIS.o \
./sdk/snc/src/snc_debug.o \
./sdk/snc/src/snc_defs.o \
./sdk/snc/src/snc_emu.o \
./sdk/snc/src/snc_hw_gpadc.o \
./sdk/snc/src/snc_hw_gpio.o \
./sdk/snc/src/snc_hw_i2c.o \
./sdk/snc/src/snc_hw_spi.o \
./sdk/snc/src/snc_hw_sys.o \
./sdk/snc/src/snc_hw_uart.o \
./sdk/snc/src/snc_main.o \
./sdk/snc/src/snc_queues.o 

C_DEPS += \
./sdk/snc/src/SeNIS.d \
./sdk/snc/src/snc_debug.d \
./sdk/snc/src/snc_defs.d \
./sdk/snc/src/snc_emu.d \
./sdk/snc/src/snc_hw_gpadc.d \
./sdk/snc/src/snc_hw_gpio.d \
./sdk/snc/src/snc_hw_i2c.d \
./sdk/snc/src/snc_hw_spi.d \
./sdk/snc/src/snc_hw_sys.d \
./sdk/snc/src/snc_hw_uart.d \
./sdk/snc/src/snc_main.d \
./sdk/snc/src/snc_queues.d 


# Each subdirectory must supply rules for building sources it contributes
sdk/snc/src/SeNIS.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src/SeNIS.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Werror -Wall  -g3 -DRELEASE_BUILD -Ddg_configDEVICE=DA14699_00 -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/util/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/portable/GCC/DA1469x" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src" -include"/home/jon/DA1469x/repos/forked_ble_sdk10_examples/BLE_SDK10_examples/features/dlg_reset_info/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/snc/src/snc_debug.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src/snc_debug.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Werror -Wall  -g3 -DRELEASE_BUILD -Ddg_configDEVICE=DA14699_00 -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/util/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/portable/GCC/DA1469x" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src" -include"/home/jon/DA1469x/repos/forked_ble_sdk10_examples/BLE_SDK10_examples/features/dlg_reset_info/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/snc/src/snc_defs.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src/snc_defs.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Werror -Wall  -g3 -DRELEASE_BUILD -Ddg_configDEVICE=DA14699_00 -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/util/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/portable/GCC/DA1469x" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src" -include"/home/jon/DA1469x/repos/forked_ble_sdk10_examples/BLE_SDK10_examples/features/dlg_reset_info/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/snc/src/snc_emu.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src/snc_emu.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Werror -Wall  -g3 -DRELEASE_BUILD -Ddg_configDEVICE=DA14699_00 -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/util/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/portable/GCC/DA1469x" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src" -include"/home/jon/DA1469x/repos/forked_ble_sdk10_examples/BLE_SDK10_examples/features/dlg_reset_info/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/snc/src/snc_hw_gpadc.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src/snc_hw_gpadc.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Werror -Wall  -g3 -DRELEASE_BUILD -Ddg_configDEVICE=DA14699_00 -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/util/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/portable/GCC/DA1469x" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src" -include"/home/jon/DA1469x/repos/forked_ble_sdk10_examples/BLE_SDK10_examples/features/dlg_reset_info/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/snc/src/snc_hw_gpio.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src/snc_hw_gpio.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Werror -Wall  -g3 -DRELEASE_BUILD -Ddg_configDEVICE=DA14699_00 -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/util/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/portable/GCC/DA1469x" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src" -include"/home/jon/DA1469x/repos/forked_ble_sdk10_examples/BLE_SDK10_examples/features/dlg_reset_info/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/snc/src/snc_hw_i2c.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src/snc_hw_i2c.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Werror -Wall  -g3 -DRELEASE_BUILD -Ddg_configDEVICE=DA14699_00 -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/util/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/portable/GCC/DA1469x" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src" -include"/home/jon/DA1469x/repos/forked_ble_sdk10_examples/BLE_SDK10_examples/features/dlg_reset_info/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/snc/src/snc_hw_spi.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src/snc_hw_spi.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Werror -Wall  -g3 -DRELEASE_BUILD -Ddg_configDEVICE=DA14699_00 -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/util/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/portable/GCC/DA1469x" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src" -include"/home/jon/DA1469x/repos/forked_ble_sdk10_examples/BLE_SDK10_examples/features/dlg_reset_info/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/snc/src/snc_hw_sys.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src/snc_hw_sys.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Werror -Wall  -g3 -DRELEASE_BUILD -Ddg_configDEVICE=DA14699_00 -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/util/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/portable/GCC/DA1469x" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src" -include"/home/jon/DA1469x/repos/forked_ble_sdk10_examples/BLE_SDK10_examples/features/dlg_reset_info/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/snc/src/snc_hw_uart.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src/snc_hw_uart.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Werror -Wall  -g3 -DRELEASE_BUILD -Ddg_configDEVICE=DA14699_00 -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/util/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/portable/GCC/DA1469x" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src" -include"/home/jon/DA1469x/repos/forked_ble_sdk10_examples/BLE_SDK10_examples/features/dlg_reset_info/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/snc/src/snc_main.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src/snc_main.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Werror -Wall  -g3 -DRELEASE_BUILD -Ddg_configDEVICE=DA14699_00 -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/util/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/portable/GCC/DA1469x" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src" -include"/home/jon/DA1469x/repos/forked_ble_sdk10_examples/BLE_SDK10_examples/features/dlg_reset_info/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/snc/src/snc_queues.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src/snc_queues.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Werror -Wall  -g3 -DRELEASE_BUILD -Ddg_configDEVICE=DA14699_00 -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/util/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/portable/GCC/DA1469x" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src" -include"/home/jon/DA1469x/repos/forked_ble_sdk10_examples/BLE_SDK10_examples/features/dlg_reset_info/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


