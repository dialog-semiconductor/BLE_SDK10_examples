# This target will get run if just `make` is used. Be sure to set the BUILD_CONFIG to one of BUILD_CONFIG_TYPES
default: all

-include makefile.init

SDKROOT := $(SOURCEROOT)/sdk

DEVICE := DEVICE_DA1469x

BUILD_CONFIG ?=  # one of BUILD_CONFIG_TYPES below
BUILD_CONFIG_TYPES := debug_qspi 
BUILD_CONFIG_TYPES += debug_qspi_suota 
BUILD_CONFIG_TYPES += debug_ram 
BUILD_CONFIG_TYPES += release_qspi 
BUILD_CONFIG_TYPES += release_qspi_suota
BUILD_CONFIG_TYPES += release_ram

BUILD_OUTPUT_DIR = $(BUILD_CONFIG)

# The gcc path below needs to be modified to match that of the user. Below uses the gcc version included with Smart Snippets Studio. Additionally, the path below is expecting to be called from a cygwin terminal 
TOOLDIR ?= /cygdrive/c/DiaSemi/SmartSnippetsStudio2.0.20/GCC/7-2018-q2/
CC := $(TOOLDIR)/bin/arm-none-eabi-gcc.exe
BIN := $(TOOLDIR)/bin/arm-none-eabi-objcopy.exe
SIZE := $(TOOLDIR)/bin/arm-none-eabi-size.exe

RM := rm -rf

# The python path below needs to be modified to match that of the user. Below uses the python version included with Smart Snippets Studio. Additionally, the path below is expecting to be called from a cygwin terminal 
PYTHON ?= /cygdrive/c/DiaSemi/SmartSnippetsStudio2.0.20/Python35/python.exe
PYTHON_SCRIPTS_DIR :=  $(SOURCEROOT)/utilities/python_scripts
CREATE_HEADER_SCRIPT := $(PYTHON_SCRIPTS_DIR)/qspi/create_flash_image.py
HEADER_CONFIG_XML ?= $(PYTHON_SCRIPTS_DIR)/qspi/program_qspi.xml # The post build step for qspi images requires a .xml flash configuration to be defined. One can be created using  <SDK_ROOT>/utilities/python_scripts/qspi/program_qpsi_config.py

ifeq (ram, $(word 2,$(subst _, ,$(BUILD_CONFIG))))
APP_CONFIG_H = "$(CONFIG_SOURCE_DIR)/custom_config_ram.h"
else ifeq (suota, $(word 3,$(subst _, ,$(BUILD_CONFIG))))
APP_CONFIG_H = "$(CONFIG_SOURCE_DIR)/custom_config_qspi_suota.h"
else
APP_CONFIG_H = "$(CONFIG_SOURCE_DIR)/custom_config_qspi.h"
endif

# Here all the sources are included. The auto-generated makefile does this
# By creating makefiles for each subdirectory, and then including them all.
# But in our case, all these subdirectory makefiles are essentially identical,
# just with different source files. So, instead, we just pull in all the 
# source files here.

ELF_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
C_SRCS := 
S_UPPER_SRCS := 
O_SRCS := 
OBJS :=
SECONDARY_FLASH :=
SECONDARY_SIZE := 
ASM_DEPS := 
S_UPPER_DEPS := 
C_DEPS :=

################################################################################
# C Source Files
################################################################################

# Header include directories
C_INCLUDE_DIRS := \
$(SDKROOT)/middleware/adapters/include \
$(SDKROOT)/interfaces/ble/adapter/include \
$(SDKROOT)/interfaces/ble/api/include \
$(SDKROOT)/interfaces/ble/clients/include \
$(SDKROOT)/interfaces/ble/config \
$(SDKROOT)/interfaces/ble/manager/include \
$(SDKROOT)/interfaces/ble/services/include \
$(SDKROOT)/interfaces/ble/stack/config \
$(SDKROOT)/interfaces/ble/stack/da14690/include \
$(SDKROOT)/bsp/include \
$(SDKROOT)/bsp/config \
$(SDKROOT)/free_rtos/include \
$(SDKROOT)/free_rtos/portable/GCC/DA1469x \
$(SDKROOT)/bsp/memory/include \
$(SDKROOT)/middleware/config \
$(SDKROOT)/middleware/osal \
$(SDKROOT)/bsp/peripherals/include \
$(SDKROOT)/bsp/snc/include \
$(SDKROOT)/bsp/snc/src \
$(SDKROOT)/bsp/system/sys_man \
$(SDKROOT)/bsp/system/sys_man/include \
$(SDKROOT)/bsp/util/include \

USER_DIRS := $(wildcard $(addsuffix /, $(APP_SOURCE_DIR))*/) # Have issue when attempt to use APP_SOURCE_DIR directly, using addsuffix as a work around
# Get directories with h files
USER_DIRS_WITH_H_FILE := $(subst //,,$(foreach d,$(USER_DIRS),$(dir $(wildcard  $d/*.h))))
USER_INCLUDES := $(sort $(USER_DIRS_WITH_H_FILE)) 
USER_INCLUDES += $(APP_SOURCE_DIR)
# Add user includes
C_INCLUDE_DIRS += $(USER_INCLUDES)

# SDK subdirectories with source files
SDK_SOURCE_SUBDIRS := bsp/startup/DA1469x
SDK_SOURCE_SUBDIRS += bsp/startup
SDK_SOURCE_SUBDIRS += bsp/util/src 
SDK_SOURCE_SUBDIRS += bsp/system/sys_man 
SDK_SOURCE_SUBDIRS += bsp/snc/src 
SDK_SOURCE_SUBDIRS += bsp/peripherals/src 
SDK_SOURCE_SUBDIRS += middleware/osal 
SDK_SOURCE_SUBDIRS += bsp/memory/src
SDK_SOURCE_SUBDIRS += interfaces/ble/stack/da14690/src 
SDK_SOURCE_SUBDIRS += interfaces/ble/services/src 
SDK_SOURCE_SUBDIRS += interfaces/ble/manager/src 
SDK_SOURCE_SUBDIRS += interfaces/ble/clients/src
SDK_SOURCE_SUBDIRS += interfaces/ble/api/src 
SDK_SOURCE_SUBDIRS += interfaces/ble/adapter/src 
SDK_SOURCE_SUBDIRS += middleware/adapters/src
SDK_SOURCE_SUBDIRS += free_rtos/portable/MemMang
SDK_SOURCE_SUBDIRS += free_rtos/portable/GCC/DA1469x 
SDK_SOURCE_SUBDIRS += free_rtos/portable/Common
SDK_SOURCE_SUBDIRS += free_rtos

# Optimization flags
OPT += -g3

ifeq (debug, $(word 1,$(subst _, ,$(BUILD_CONFIG))))
	OPT += -Og
else
	OPT += -Os
endif

# Compilation flags
COMMON_FLAGS += -mcpu=cortex-m33
COMMON_FLAGS += -mthumb
COMMON_FLAGS += -mfloat-abi=hard
COMMON_FLAGS += -mfpu=fpv5-sp-d16
COMMON_FLAGS += -fmessage-length=0
COMMON_FLAGS += -fsigned-char
COMMON_FLAGS += -ffunction-sections -fdata-sections
COMMON_FLAGS += -Werror -Wall

CFLAGS += $(OPT) $(COMMON_FLAGS)
CFLAGS += -Ddg_configDEVICE=$(DEVICE)
CFLAGS += -include$(APP_CONFIG_H)
CFLAGS += -MMD -MP

# Linker Flags
LDFLAGS += $(COMMON_FLAGS)
LDFLAGS += -Og
LDFLAGS += -g3		
LDFLAGS += -T "$(BUILD_OUTPUT_DIR)/mem.ld" 
LDFLAGS += -T "$(BUILD_OUTPUT_DIR)/sections.ld" 
LDFLAGS += -Xlinker --gc-sections
LDFLAGS += -L"$(SDKROOT)/bsp/ldscripts/ble_projects"
LDFLAGS += -L"$(SDKROOT)/interfaces/ble/binaries/DA1469x-Release"

# Go through all the SDK source subdirectories and pull in all files that end in ".c"
SDK_C_SRCS := $(wildcard $(addsuffix /*.c, $(addprefix $(SDKROOT)/, $(SDK_SOURCE_SUBDIRS))))

# Recursive wildcard:  
# https://stackoverflow.com/questions/2483182/recursive-wildcards-in-gnu-make/18258352#18258352
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

# Go through all the files and subdirectories in app_src and pull in all c source files.
APP_C_SRCS := $(call rwildcard,$(APP_SOURCE_DIR),*.c)
APP_C_SRCS += $(call rwildcard,$(CONFIG_SOURCE_DIR),*.c)

C_SRCS = $(strip $(SDK_C_SRCS) $(APP_C_SRCS))

################################################################################
# .S Assembly Source Files
################################################################################
# This section is analogous to the one below, but for assembly files.

# Header include directories
S_UPPER_INCLUDE_DIRS := $(SDKROOT)/bsp/config
S_UPPER_INCLUDE_DIRS += $(SDKROOT)/middleware/config
S_UPPER_INCLUDE_DIRS += $(CONFIG_SOURCE_DIR)

S_UPPER_SRCS = $(wildcard $(SDKROOT)/bsp/startup/DA1469x/GCC/*.S)

# Generate a rule to build a .o and a .d file for every .S file. See C_DEPENDENCY_template
define S_UPPER_DEPENDENCY_template
$(2): $(1) 
	@echo 'Building file: $(1)'
	@echo 'Invoking: Cross ARM GNU Assembler'

# Create the appropriate folder inside the build folder, if it doesn't exist already.
	@mkdir -p $(dir $(2))

	@$(CC) $(CFLAGS) -x assembler-with-cpp \
	$(addprefix -I, $(S_UPPER_INCLUDE_DIRS)) \
	-MF"$(2:%.o=%.d)" \
	-MT"$(2)" \
	-c -o "$(2)" "$(1)"

	@echo 'Finished building: $(1)'
	@echo ' '

OBJS += $(2)
S_UPPER_DEPS += $(patsubst %.o,%.d,$(2))
endef

# This macro takes a c file as input and generates a rule for building it. Not only does
# the rule build the associated .o file (which is the target of the rule), but it also 
# builds a _makefile_ that lists all the _header_ dependencies of the .o file. 
# This file has the extension ".d". When the makefile is run for the first time, no .o
# files exist, so this rule gets run for every .c file and generates a .o and a .d file for each.
# On subsequent executions, the makefile imports all the .d files. This means that the .o
# files will rebuilt if _either_ their .c file _or any header dependency_ is changed.
#
# The input c file is passed in as $(1). The .o and .d files are generated from $(1). 
define C_DEPENDENCY_template
$(2): $(1) 
	@echo 'Building file: $(1)'
	@echo 'Invoking: Cross ARM C Compiler'

# Create the appropriate folder inside the build folder, if it doesn't exist already.
	@mkdir -p $(dir $(2))

	@$(CC) $(CFLAGS) \
	$(addprefix -I, $(C_INCLUDE_DIRS)) \
	-std=gnu11 \
	-MF"$(2:%.o=%.d)" \
	-MT"$(2)" \
	-c -o "$(2)" "$(1)"

	@echo 'Finished building: $(1)'
	@echo ' '

OBJS += $(2)
C_DEPS += $(patsubst %.o,%.d,$(2))
endef

# Convienent mechanisms to call above dependency templates
s_template=$(eval $(call S_UPPER_DEPENDENCY_template,$(1),$(subst $(SOURCEROOT)/,$(BUILD_OUTPUT_DIR)/,$(patsubst %.S,%.o,$(1)))))
c_sdk_template=$(eval $(call C_DEPENDENCY_template,$(1),$(subst $(SOURCEROOT)/,$(BUILD_OUTPUT_DIR)/,$(patsubst %.c,%.o,$(1))))) 
# This is used to specify a different output folder for artifacts produced from building user sources
c_user_src_template=$(eval $(call C_DEPENDENCY_template,$(1),$(subst $(addsuffix /, $(APP_SOURCE_DIR)),$(BUILD_OUTPUT_DIR)/user/,$(patsubst %.c,%.o,$(1))))) 

# Generate a rule to build a .o and a .d file for every .S file.
$(foreach src,$(S_UPPER_SRCS),$(call s_template,$(src)))

# Generate a rule to build a .o and a .d file for every .c file.
$(foreach src, $(SDK_C_SRCS),$(call c_sdk_template,$(src)))
$(foreach src, $(APP_C_SRCS),$(call c_user_src_template,$(src)))


################################################################################
# Import Dependencies
################################################################################

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

USER_OBJS :=
LIBS := -lble_stack_da1469x 

# Templates for cleaning 
define clean_extensions_template
	@find . -wholename */$(strip $(1))/*.elf -type f -delete
	@find . -wholename */$(strip $(1))/*.ld -type f -delete
	@find . -wholename */$(strip $(1))/*.map -type f -delete
	@find . -wholename */$(strip $(1))/*.img -type f -delete
	@find . -wholename */$(strip $(1))/*.bin -type f -delete
	@find . -wholename */$(strip $(1))/*.prev -type f -delete
endef

define clean_template

	@echo 'Cleaning $(strip $(1))'
	$(call clean_extensions_template, $(1))
	-@$(RM) \
	$(addprefix $(strip $(1)),$(OBJS)) \
	$(SECONDARY_FLASH) \
	$(SECONDARY_SIZE) \
	$(addprefix $(strip $(1)),$(ASM_DEPS)) \
	$(addprefix $(strip $(1)),$(S_UPPER_DEPS)) \
	$(addprefix $(strip $(1)),$(C_DEPS)) 
	@echo 'Finished cleaning $(strip $(1))'
	@echo ' '

endef

################################################################################
# Rules
################################################################################

# Allow extra symbols to be defined before compilation. Optional file.
-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \

BUILD_ARTIFACT_NAME ?= pxp_reporter_makefile
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

SECONDARY_FLASH += \
$(BUILD_ARTIFACT_NAME).bin \

SECONDARY_SIZE += \
$(BUILD_ARTIFACT_NAME).siz \


all: pre-build main-build

# Main-build Target
main-build: check-build-config $(BUILD_ARTIFACT) secondary-outputs 
 
# Tool invocations

# Generate .elf 
$(BUILD_ARTIFACT): $(OBJS) $(USER_OBJS) $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C Linker'
	@$(CC) $(LDFLAGS) \
	 -Wl,-Map,"$(BUILD_OUTPUT_DIR)/$(BUILD_ARTIFACT_NAME).map" \
	--specs=nano.specs --specs=nosys.specs -o "$(BUILD_OUTPUT_DIR)/$@" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Generate .bin 
$(SECONDARY_FLASH): $(BUILD_ARTIFACT) $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	@$(BIN) -O binary "$(BUILD_OUTPUT_DIR)/$<" "$(BUILD_OUTPUT_DIR)/$@"
	@echo 'Finished building: $@'
	@echo ' '

# Print size
$(SECONDARY_SIZE): $(BUILD_ARTIFACT) $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: Cross ARM GNU Print Size'
	@$(SIZE) --format=berkeley "$(BUILD_OUTPUT_DIR)/$<"
	@echo 'Finished building: $@'
	@echo ' '

# make clean will clean ALL build configurations {debug_qpsi, debug_qpsi_suota, debug_ram, release_qspi, release_qspi_suota, release_ram}
# setting BUILD_CONFIG when calling clean will clean only that configuration
# For example:
# make BUILD_CONFIG=debug_qspi clean 
# will clean only the debug_qspi build
clean:
ifeq (,$(BUILD_CONFIG))
	@echo 'Cleaning all build configuration outputs'
	@echo ' '
	$(foreach config,$(BUILD_CONFIG_TYPES),$(call clean_template,$(config)))
	@echo 'Finished cleaning all build configurations'
	@echo ' '

else
	@echo 'Cleaning $(BUILD_CONFIG) build outputs'
	$(call clean_extensions_template, $(BUILD_CONFIG))
	-@$(RM) $(OBJS) $(SECONDARY_FLASH) $(SECONDARY_SIZE) $(ASM_DEPS) $(S_UPPER_DEPS) $(C_DEPS)
	@echo 'Finished cleaning $(BUILD_CONFIG)'
	@echo ' '

endif

# Check that a valid build configuration has been selected
check-build-config:
ifeq (,$(findstring $(BUILD_CONFIG), $(BUILD_CONFIG_TYPES)))
	$(error Invalid build configuration. Please set BUILD_CONFIG to one of {$(BUILD_CONFIG_TYPES)})
endif

# Generate linker scripts
pre-build: check-build-config
	-@echo 'Generate linker scripts.'
	-@mkdir -p $(BUILD_OUTPUT_DIR)
	@make generate_ldscripts DEVICE=$(DEVICE) PRE_BUILD_EXTRA_DEFS="" \
	APP_CONFIG_H=$(APP_CONFIG_H) \
	CC=$(CC) \
	BSP_CONFIG_DIR="$(SDKROOT)/bsp/config" \
	MIDDLEWARE_CONFIG_DIR="$(SDKROOT)/middleware/config" \
	LDSCRIPT_PATH="$(SDKROOT)/bsp/ldscripts/ble_projects"
	-@echo ' '
			
# For qspi builds, create a .img with product and image header. 
post-build: main-build
	# Create .img with product and image headers
	@$(PYTHON) $(CREATE_HEADER_SCRIPT) --header_config $(HEADER_CONFIG_XML) --image $(BUILD_OUTPUT_DIR)/$(BUILD_ARTIFACT_NAME).img $(BUILD_OUTPUT_DIR)/$(BUILD_ARTIFACT_NAME).bin

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents main-build pre-build post-build

# Define custom build targets. For this application, this imports a rule to build the linker scrips.
-include ../makefile.targets
