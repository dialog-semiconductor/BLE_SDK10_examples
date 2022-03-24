################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include startup/DA1469x/GCC/subdir.mk
-include startup/DA1469x/subdir.mk
-include startup/subdir.mk
-include sdk/util/src/subdir.mk
-include sdk/sys_man/subdir.mk
-include sdk/snc/src/subdir.mk
-include sdk/peripherals/src/subdir.mk
-include sdk/osal/subdir.mk
-include sdk/memory/src/subdir.mk
-include sdk/adapters/src/subdir.mk
-include sdk/FreeRTOS/portable/MemMang/subdir.mk
-include sdk/FreeRTOS/portable/GCC/DA1469x/subdir.mk
-include sdk/FreeRTOS/portable/Common/subdir.mk
-include sdk/FreeRTOS/subdir.mk
-include app_debug_freertos/src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
freertos_retarget.bin \

SECONDARY_SIZE += \
freertos_retarget.siz \


# All Target
all:
	$(MAKE) --no-print-directory pre-build
	$(MAKE) --no-print-directory main-build

# Main-build Target
main-build: freertos_retarget.elf secondary-outputs

# Tool invocations
freertos_retarget.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Werror -Wall  -g3 -T "/home/jon/DA1469x/repos/forked_ble_sdk10_examples/BLE_SDK10_examples/features/dlg_reset_info/DA1469x-00-Release_QSPI/mem.ld" -T "/home/jon/DA1469x/repos/forked_ble_sdk10_examples/BLE_SDK10_examples/features/dlg_reset_info/DA1469x-00-Release_QSPI/sections.ld" -Xlinker --gc-sections -Wl,-Map,"freertos_retarget.map" --specs=nano.specs --specs=nosys.specs -o "freertos_retarget.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

freertos_retarget.bin: freertos_retarget.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O binary "freertos_retarget.elf"  "freertos_retarget.bin"
	@echo 'Finished building: $@'
	@echo ' '

freertos_retarget.siz: freertos_retarget.elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley "freertos_retarget.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS) freertos_retarget.elf
	-@echo ' '

pre-build:
	-@echo 'Generate linker scripts.'
	-make generate_ldscripts DEVICE=DA14699_00 LD_DEFS=-DRELEASE_BUILD APP_CONFIG_H="/home/jon/DA1469x/repos/forked_ble_sdk10_examples/BLE_SDK10_examples/features/dlg_reset_info/config/custom_config_qspi.h" CC="arm-none-eabi-gcc" BSP_CONFIG_DIR="/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/config" MIDDLEWARE_CONFIG_DIR="/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/middleware/config" LDSCRIPT_PATH="/home/jon/DA1469x/SDKs/SDK_10.0.10.118.3_test/SDK_10.0.10.118.3/sdk/bsp/ldscripts/non_ble_projects"
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents main-build pre-build

-include ../makefile.targets
