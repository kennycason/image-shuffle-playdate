HEAP_SIZE      = 8388208
STACK_SIZE     = 61800

PRODUCT = build/ImageShuffle.pdx

# Locate the SDK
SDK = ${PLAYDATE_SDK_PATH}
ifeq ($(SDK),)
SDK = $(shell egrep '^\s*SDKRoot' ~/.Playdate/config | head -n 1 | cut -c9-)
endif

ifeq ($(SDK),)
$(error SDK path not found; set ENV value PLAYDATE_SDK_PATH)
endif

VPATH += Source

# List C source files here
SRC = 	\
        main.c

# List all user directories here
UINCDIR =

# List user asm files
UASRC = 

# List all user C define here, like -D_DEBUG=1
UDEFS = 

# Define ASM defines here
UADEFS = 

# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS =

include $(SDK)/C_API/buildsupport/common.mk

# Run in Simulator
run: $(PRODUCT)
	open -a "Playdate Simulator" $(PRODUCT)

# .PHONY declarations
.PHONY: clean run