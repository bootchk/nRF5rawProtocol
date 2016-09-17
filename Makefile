# lkk notes
# see my hacks at 'lkk', everthing else is from example blinky Makefile
# c++ but use .c for file suffixes, Makefile does not support .cpp suffix
# derived from project blinky_blank_pca10040

export OUTPUT_FILENAME
#MAKEFILE_NAME := $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
MAKEFILE_NAME := $(MAKEFILE_LIST)
MAKEFILE_DIR := $(dir $(MAKEFILE_NAME) ) 

# lkk hack
NRF_SDK_ROOT = /home/bootch/nrf5_sdk
NRF_SDK_LIBS = /home/bootch/nrf5_sdk/components/libraries
NRF_SDK_DRVS = /home/bootch/nrf5_sdk/components/drivers_nrf
TEMPLATE_PATH = $(NRF_SDK_ROOT)/components/toolchain/gcc

ifeq ($(OS),Windows_NT)
include $(TEMPLATE_PATH)/Makefile.windows
else
include $(TEMPLATE_PATH)/Makefile.posix
endif

MK := mkdir
RM := rm -rf

#echo suspend
ifeq ("$(VERBOSE)","1")
NO_ECHO := 
else
NO_ECHO := @
endif

# Toolchain commands
#lkk was gcc
CC              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-g++'
AS              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-as'
AR              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ar' -r
LD              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ld'
NM              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-nm'
OBJDUMP         := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objdump'
OBJCOPY         := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objcopy'
SIZE            := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-size'

#function for removing duplicates in a list
remduplicates = $(strip $(if $1,$(firstword $1) $(call remduplicates,$(filter-out $(firstword $1),$1))))

# Source from the SDK, needed by every nrf project
C_SOURCE_FILES += $(abspath $(TEMPLATE_PATH)/../system_nrf52.c)

# !!! other Nordic source files used by app CAN be soft links created in virtual folders e.g. ./Device or ./nrfLibraries
# (but also need to be declared as sources using the link name)
# OR as here defined directly

# libraries group
C_SOURCE_FILES +=  $(NRF_SDK_LIBS)/timer/app_timer.c
C_SOURCE_FILES +=  $(NRF_SDK_LIBS)/util/app_error.c
C_SOURCE_FILES +=  $(NRF_SDK_LIBS)/util/app_util_platform.c

# drivers group
C_SOURCE_FILES +=  $(NRF_SDK_DRVS)/clock/nrf_drv_clock.c
C_SOURCE_FILES +=  $(NRF_SDK_DRVS)/common/nrf_drv_common.c

# logging
#C_SOURCE_FILES +=  $(NRF_SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c
#C_SOURCE_FILES +=  $(NRF_SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c


# lkk hack
# other source of my devising, some in a hard folder: modules
# Note that nrf_delay.c does not exist, implemented entirely in nrf_delay.h
C_SOURCE_FILES +=  main.c
C_SOURCE_FILES +=  modules/radio.c
C_SOURCE_FILES +=  modules/radioLowLevel.c
C_SOURCE_FILES +=  modules/radioConfigure.c
C_SOURCE_FILES +=  modules/transport.c
C_SOURCE_FILES +=  modules/timer.c
C_SOURCE_FILES +=  modules/irqHandlers.c
C_SOURCE_FILES +=  modules/hardFaultHandler.c
C_SOURCE_FILES +=  modules/hfClock.c

#assembly files common to all targets
#lkk this file is linked linked resource in Eclipse, but not a linked file in Linux
#lkk was lower case .s
ASM_SOURCE_FILES  = $(abspath $(TEMPLATE_PATH)/gcc_startup_nrf52.s)
# ASM_SOURCE_FILES  = gcc_startup_nrf52.s
#lkk
#ASM_SOURCE_FILES += modules/hardFaultHandler.s

#includes common to all targets
#lkk !!! Case sensitive, and since the SDK comes from Windows case insensitive, often SDK has vagaries of capitalization?
INC_PATHS += -I$(abspath $(NRF_SDK_ROOT)/components/toolchain/gcc)
INC_PATHS += -I$(abspath $(NRF_SDK_ROOT)/components/toolchain)
#lkk I don't understand why this was here
#INC_PATHS += -I$(abspath ../../..)
INC_PATHS += -I$(abspath $(NRF_SDK_ROOT)/examples/bsp)
INC_PATHS += -I$(abspath $(NRF_SDK_ROOT)/components/device)
# lkk not using delay
#INC_PATHS += -I$(abspath $(NRF_SDK_ROOT)/components/drivers_nrf/delay)
#lkk v11 capitalization was INC_PATHS += -I$(abspath $(NRF_SDK_ROOT)/components/toolchain/CMSIS/Include)
INC_PATHS += -I$(abspath $(NRF_SDK_ROOT)/components/toolchain/cmsis/include)
INC_PATHS += -I$(abspath $(NRF_SDK_DRVS)/hal)

#includes specific to this project

# lkk using timer library: the following chain discovered by trial and error starting with #include "app_timer.h" in main
INC_PATHS += -I$(abspath $(NRF_SDK_LIBS)/timer)
# lkk timer lib depends on sdk_config.h, which I put in 
INC_PATHS += -I$(abspath .)
# lkk app_timer depends on app_error.h found here:
INC_PATHS += -I$(abspath $(NRF_SDK_LIBS)/util)
# lkk sdk_errors depends on nrf_error.h, found here
INC_PATHS += -I$(abspath $(NRF_SDK_DRVS)/nrf_soc_nosd)
# lkk app_timer uses nrf_drv_clock.h, found here
INC_PATHS += -I$(abspath $(NRF_SDK_DRVS)/clock)
# lkk nrf_drv_clock.h, depends on nrf_drv_common.h, found here
INC_PATHS += -I$(abspath $(NRF_SDK_DRVS)/common)
# lkk app_error.c, depends on nrf_logon.h, found here
INC_PATHS += -I$(abspath $(NRF_SDK_LIBS)/log)
# lkk nrf_log.h, depends on nrf_log_inhternal.h, found here
INC_PATHS += -I$(abspath $(NRF_SDK_LIBS)/log/src)
# lkk app_timer.c, depends on nrf_delay.h, found here
INC_PATHS += -I$(abspath $(NRF_SDK_DRVS)/delay)



OBJECT_DIRECTORY = _build
LISTING_DIRECTORY = $(OBJECT_DIRECTORY)
OUTPUT_BINARY_DIRECTORY = $(OBJECT_DIRECTORY)

# Sorting removes duplicates
BUILD_DIRECTORIES := $(sort $(OBJECT_DIRECTORY) $(OUTPUT_BINARY_DIRECTORY) $(LISTING_DIRECTORY) )

#flags common to all targets
CFLAGS  = -DNRF52_PAN_12
CFLAGS += -DNRF52_PAN_15
CFLAGS += -DNRF52_PAN_58
CFLAGS += -DNRF52_PAN_20
CFLAGS += -DNRF52_PAN_54
CFLAGS += -DNRF52_PAN_31
CFLAGS += -DNRF52_PAN_30
CFLAGS += -DNRF52_PAN_51
CFLAGS += -DNRF52_PAN_36
CFLAGS += -DNRF52_PAN_53
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DNRF52_PAN_64
CFLAGS += -DNRF52_PAN_55
CFLAGS += -DNRF52_PAN_62
CFLAGS += -DNRF52_PAN_63
CFLAGS += -DBOARD_PCA10040
CFLAGS += -DNRF52
CFLAGS += -DBSP_DEFINES_ONLY
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs 
# lkk not valid for g++: CFLAGS += --std=gnu11   original was gnu99
# lkk excise -Werror
# lkk add -fpermissive for compiling nrf C code that is non-strict
# lkk add -std=c++11 for support of nullptr
CFLAGS += -Wall -O0 -g3 -fpermissive -std=c++11
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in separate section. This will allow linker to dump unused functions
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin --short-enums 
#lkk
CFLAGS += -DDEBUG
#CFLAGS += -fshort-wchar

# keep every function in separate section. This will allow linker to dump unused functions
LDFLAGS += -Xlinker -Map=$(LISTING_DIRECTORY)/$(OUTPUT_FILENAME).map
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys

# Assembler flags
ASMFLAGS += -x assembler-with-cpp
ASMFLAGS += -DNRF52_PAN_12
ASMFLAGS += -DNRF52_PAN_15
ASMFLAGS += -DNRF52_PAN_58
ASMFLAGS += -DNRF52_PAN_20
ASMFLAGS += -DNRF52_PAN_54
ASMFLAGS += -DNRF52_PAN_31
ASMFLAGS += -DNRF52_PAN_30
ASMFLAGS += -DNRF52_PAN_51
ASMFLAGS += -DNRF52_PAN_36
ASMFLAGS += -DNRF52_PAN_53
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DNRF52_PAN_64
ASMFLAGS += -DNRF52_PAN_55
ASMFLAGS += -DNRF52_PAN_62
ASMFLAGS += -DNRF52_PAN_63
ASMFLAGS += -DBOARD_PCA10040
ASMFLAGS += -DNRF52
ASMFLAGS += -DBSP_DEFINES_ONLY

#default target - first one defined
default: clean nrf52832_xxaa

#building all targets
all: clean
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e cleanobj
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e nrf52832_xxaa

#target for printing all targets
help:
	@echo following targets are available:
	@echo 	nrf52832_xxaa

C_SOURCE_FILE_NAMES = $(notdir $(C_SOURCE_FILES))
C_PATHS = $(call remduplicates, $(dir $(C_SOURCE_FILES) ) )
C_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(C_SOURCE_FILE_NAMES:.c=.o) )

ASM_SOURCE_FILE_NAMES = $(notdir $(ASM_SOURCE_FILES))
ASM_PATHS = $(call remduplicates, $(dir $(ASM_SOURCE_FILES) ))
ASM_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(ASM_SOURCE_FILE_NAMES:.s=.o) )

vpath %.c $(C_PATHS)
vpath %.s $(ASM_PATHS)

OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

nrf52832_xxaa: OUTPUT_FILENAME := nrf52832_xxaa
nrf52832_xxaa: LINKER_SCRIPT=nRF5rawProtocol_gcc_nrf52.ld
# ble nano   use nrf51_xxaa.ld (no softdevice, 256k flash, 16k RAM)

nrf52832_xxaa: $(BUILD_DIRECTORIES) $(OBJECTS)
	@echo Linking target: $(OUTPUT_FILENAME).out
	$(NO_ECHO)$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -lm -o $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e finalize

## Create build directories
$(BUILD_DIRECTORIES):
	echo $(MAKEFILE_NAME)
	$(MK) $@

# Create objects from C SRC files
$(OBJECT_DIRECTORY)/%.o: %.c
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(CC) $(CFLAGS) $(INC_PATHS) -c -o $@ $<
	
# Assemble files
$(OBJECT_DIRECTORY)/%.o: %.s
	@echo Assembly file: $(notdir $<)
	$(NO_ECHO)$(CC) $(ASMFLAGS) $(INC_PATHS) -c -o $@ $<
# Link
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out: $(BUILD_DIRECTORIES) $(OBJECTS)
	@echo Linking target: $(OUTPUT_FILENAME).out
	$(NO_ECHO)$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -lm -o $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
## Create binary .bin file from the .out file
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	@echo Preparing: $(OUTPUT_FILENAME).bin
	$(NO_ECHO)$(OBJCOPY) -O binary $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin

## Create binary .hex file from the .out file
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	@echo Preparing: $(OUTPUT_FILENAME).hex
	$(NO_ECHO)$(OBJCOPY) -O ihex $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex

finalize: genbin genhex echosize

genbin:
	@echo Preparing: $(OUTPUT_FILENAME).bin
	$(NO_ECHO)$(OBJCOPY) -O binary $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin

## Create binary .hex file from the .out file
genhex: 
	@echo Preparing: $(OUTPUT_FILENAME).hex
	$(NO_ECHO)$(OBJCOPY) -O ihex $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex
echosize:
	-@echo ''
	$(NO_ECHO)$(SIZE) $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	-@echo ''

clean:
	$(RM) $(BUILD_DIRECTORIES)

cleanobj:
	$(RM) $(BUILD_DIRECTORIES)/*.o
flash: nrf52832_xxaa
	@echo Flashing: $(OUTPUT_BINARY_DIRECTORY)/$<.hex
	/home/bootch/Downloads/nrfjprog/nrfjprog --program $(OUTPUT_BINARY_DIRECTORY)/$<.hex -f nrf52  --chiperase
	/home/bootch/Downloads/nrfjprog/nrfjprog --reset -f nrf52
