################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/croutine.c \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/event_groups.c \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/list.c \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/queue.c \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/stream_buffer.c \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/tasks.c \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/timers.c 

OBJS += \
./sdk/FreeRTOS/croutine.o \
./sdk/FreeRTOS/event_groups.o \
./sdk/FreeRTOS/list.o \
./sdk/FreeRTOS/queue.o \
./sdk/FreeRTOS/stream_buffer.o \
./sdk/FreeRTOS/tasks.o \
./sdk/FreeRTOS/timers.o 

C_DEPS += \
./sdk/FreeRTOS/croutine.d \
./sdk/FreeRTOS/event_groups.d \
./sdk/FreeRTOS/list.d \
./sdk/FreeRTOS/queue.d \
./sdk/FreeRTOS/stream_buffer.d \
./sdk/FreeRTOS/tasks.d \
./sdk/FreeRTOS/timers.d 


# Each subdirectory must supply rules for building sources it contributes
sdk/FreeRTOS/croutine.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/croutine.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Werror -Wall  -g3 -DRELEASE_BUILD -Ddg_configDEVICE=DA14699_00 -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/util/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/portable/GCC/DA1469x" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src" -include"/home/jon/DA1469x/repos/forked_ble_sdk10_examples/BLE_SDK10_examples/features/dlg_reset_info/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/FreeRTOS/event_groups.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/event_groups.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Werror -Wall  -g3 -DRELEASE_BUILD -Ddg_configDEVICE=DA14699_00 -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/util/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/portable/GCC/DA1469x" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src" -include"/home/jon/DA1469x/repos/forked_ble_sdk10_examples/BLE_SDK10_examples/features/dlg_reset_info/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/FreeRTOS/list.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/list.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Werror -Wall  -g3 -DRELEASE_BUILD -Ddg_configDEVICE=DA14699_00 -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/util/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/portable/GCC/DA1469x" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src" -include"/home/jon/DA1469x/repos/forked_ble_sdk10_examples/BLE_SDK10_examples/features/dlg_reset_info/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/FreeRTOS/queue.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/queue.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Werror -Wall  -g3 -DRELEASE_BUILD -Ddg_configDEVICE=DA14699_00 -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/util/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/portable/GCC/DA1469x" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src" -include"/home/jon/DA1469x/repos/forked_ble_sdk10_examples/BLE_SDK10_examples/features/dlg_reset_info/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/FreeRTOS/stream_buffer.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/stream_buffer.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Werror -Wall  -g3 -DRELEASE_BUILD -Ddg_configDEVICE=DA14699_00 -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/util/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/portable/GCC/DA1469x" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src" -include"/home/jon/DA1469x/repos/forked_ble_sdk10_examples/BLE_SDK10_examples/features/dlg_reset_info/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/FreeRTOS/tasks.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/tasks.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Werror -Wall  -g3 -DRELEASE_BUILD -Ddg_configDEVICE=DA14699_00 -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/util/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/portable/GCC/DA1469x" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src" -include"/home/jon/DA1469x/repos/forked_ble_sdk10_examples/BLE_SDK10_examples/features/dlg_reset_info/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/FreeRTOS/timers.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/timers.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Werror -Wall  -g3 -DRELEASE_BUILD -Ddg_configDEVICE=DA14699_00 -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/util/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/free_rtos/portable/GCC/DA1469x" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/snc/src" -include"/home/jon/DA1469x/repos/forked_ble_sdk10_examples/BLE_SDK10_examples/features/dlg_reset_info/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


