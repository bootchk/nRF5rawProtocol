# lkk notes
# see my hacks at 'lkk', everthing else is from example blinky Makefile
# c++ but use .c for file suffixes, Makefile does not support .cpp suffix
# derived from project blinky_blank_pca10040 in SDK11 !!! Not exist anymore in v12


export OUTPUT_FILENAME
#MAKEFILE_NAME := $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
MAKEFILE_NAME := $(MAKEFILE_LIST)
MAKEFILE_DIR := $(dir $(MAKEFILE_NAME) ) 

# lkk hack
HOME = /home/bootch
NRF_SDK_ROOT = $(HOME)/nrf5_sdk
NRF_SDK_LIBS = $(HOME)/nrf5_sdk/components/libraries
NRF_SDK_DRVS = $(HOME)/nrf5_sdk/components/drivers_nrf
OTHER_TOOLS = $(HOME)/Downloads/nrfjprog
TEMPLATE_PATH = $(NRF_SDK_ROOT)/components/toolchain/gcc


include $(TEMPLATE_PATH)/Makefile.posix

MK := mkdir
RM := rm -rf

#echo suspend
ifeq ("$(VERBOSE)","1")
NO_ECHO := 
else
NO_ECHO := @
endif

# Toolchain commands
#lkk gcc => g++
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
C_SOURCE_FILES += $(abspath $(TEMPLATE_PATH)/../system_nrf51.c)



# libraries
C_SOURCE_FILES +=  $(NRF_SDK_LIBS)/timer/app_timer.c
C_SOURCE_FILES +=  $(NRF_SDK_LIBS)/util/app_error.c
C_SOURCE_FILES +=  $(NRF_SDK_LIBS)/util/app_util_platform.c

# drivers
C_SOURCE_FILES +=  $(NRF_SDK_DRVS)/clock/nrf_drv_clock.c
C_SOURCE_FILES +=  $(NRF_SDK_DRVS)/common/nrf_drv_common.c

# logging
#C_SOURCE_FILES +=  $(NRF_SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c
#C_SOURCE_FILES +=  $(NRF_SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c


# lkk hack
# other source of my devising, some in a hard folder: modules
# Note that nrf_delay.c does not exist, implemented entirely in nrf_delay.h
C_SOURCE_FILES +=  main.c
#C_SOURCE_FILES +=  testMain.c
C_SOURCE_FILES +=  wedgedMain.c

C_SOURCE_FILES +=  modules/radio.c
C_SOURCE_FILES +=  modules/radioDevice.c
C_SOURCE_FILES +=  modules/radioConfigure.c
C_SOURCE_FILES +=  modules/radioAddress.c
C_SOURCE_FILES +=  modules/hfClock.c
C_SOURCE_FILES +=  modules/system.c

C_SOURCE_FILES +=  platform/timer.c
C_SOURCE_FILES +=  platform/irqHandlers.c
C_SOURCE_FILES +=  platform/hardFaultHandler.c
C_SOURCE_FILES +=  platform/ledLogger.c
C_SOURCE_FILES +=  platform/uniqueID.c
C_SOURCE_FILES +=  platform/sleeper.c



#$(abspath ../../../main.c) \
#$(abspath ../../../../../../components/drivers_nrf/delay/nrf_delay.c) \

#assembly files common to all targets
ASM_SOURCE_FILES  = $(abspath $(TEMPLATE_PATH)/gcc_startup_nrf51.s)




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
CFLAGS  = -DNRF51
#lkk declare a custom board
#CFLAGS += -DBOARD_PCA10028
# BLE Nano or other.  Uses custom_board.h in project via boards.h in SDK
CFLAGS += -DBOARD_CUSTOM
CFLAGS += -DBSP_DEFINES_ONLY
CFLAGS += -mcpu=cortex-m0
CFLAGS += -mthumb -mabi=aapcs 

# lkk not valid for g++: CFLAGS += --std=gnu11   original was gnu99
# lkk excise -Werror
# lkk add -fpermissive for compiling nrf C code that is non-strict
# lkk add -std=c++11 for support of nullptr
CFLAGS += -fpermissive -std=c++11

CFLAGS += -Wall -O3 -g3
CFLAGS += -mfloat-abi=soft
# keep every function in separate section. This will allow linker to dump unused functions
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin --short-enums 

#lkk
CFLAGS += -DDEBUG

# keep every function in separate section. This will allow linker to dump unused functions
LDFLAGS += -Xlinker -Map=$(LISTING_DIRECTORY)/$(OUTPUT_FILENAME).map
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m0
# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections

#lkk suppress warnings about wchar 4 or 2
LDFLAGS += -Wl,--no-wchar-size-warning

# Linker flags for libraries
# use newlib version of std C libc, in nano version
STDC_LIBS += --specs=nano.specs -lc -lnosys
# My own static lib
MY_LIBS += -L . -lsleepSyncAgent
# libmath.a  Math lib is separate from libc.  
# Math lib automatically include if use libstdc++, but we are not necessarily using libstdc++??
MATH_LIBS += -lm
LIBS += $(STDC_LIBS) $(MY_LIBS) $(MATH_LIBS)

# Assembler flags
ASMFLAGS += -x assembler-with-cpp
ASMFLAGS += -DNRF51
ASMFLAGS += -DBOARD_PCA10028
ASMFLAGS += -DBSP_DEFINES_ONLY

#default target - first one defined
default: clean nrf51422_xxac

#building all targets
all: clean
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e cleanobj
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e nrf51422_xxac

#target for printing all targets
help:
	@echo following targets are available:
	@echo 	nrf51422_xxac

C_SOURCE_FILE_NAMES = $(notdir $(C_SOURCE_FILES))
C_PATHS = $(call remduplicates, $(dir $(C_SOURCE_FILES) ) )
C_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(C_SOURCE_FILE_NAMES:.c=.o) )

ASM_SOURCE_FILE_NAMES = $(notdir $(ASM_SOURCE_FILES))
ASM_PATHS = $(call remduplicates, $(dir $(ASM_SOURCE_FILES) ))
ASM_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(ASM_SOURCE_FILE_NAMES:.s=.o) )

vpath %.c $(C_PATHS)
vpath %.s $(ASM_PATHS)

OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

nrf51422_xxac: OUTPUT_FILENAME := nrf51422_xxac
nrf51422_xxac: LINKER_SCRIPT=gcc_nrf51.ld

nrf51422_xxac: $(BUILD_DIRECTORIES) $(OBJECTS)
	@echo Linking target: $(OUTPUT_FILENAME).out
	$(NO_ECHO)$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
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
	@echo Linking target: $@
	$(NO_ECHO)$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@
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
flash: nrf51422_xxac
	@echo Flashing: $(OUTPUT_BINARY_DIRECTORY)/$<.hex
	$(OTHER_TOOLS)/nrfjprog --program $(OUTPUT_BINARY_DIRECTORY)/$<.hex -f nrf51  --chiperase
	$(OTHER_TOOLS)/nrfjprog --reset -f nrf51

## Flash softdevice