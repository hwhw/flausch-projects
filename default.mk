# IMPORTANT: file 'procdefs.ld' absolutely must Must MUST be present in the
# working directory when compiling code for the UBW32!  Failure to do so will
# almost certainly result in your UBW32 getting 'bricked' and requiring
# re-flashing the bootloader (which, if you don't have a PICkit 2 or similar
# PIC programmer, you're screwed).  YOU HAVE BEEN WARNED.

# Type 'make' to build example program.  Put UBW32 device in Bootloader mode
# (PRG + RESET buttons), then type 'make write' to download program to the
# device.  Note: the latter will OVERWRITE any program you currently have
# installed there, including the Bit Whacker firmware, so be sure to keep
# around a copy of the original, which can be downloaded from the product
# web page here: http://www.schmalzhaus.com/UBW32/


# MCU to compile for:
PROC    = 32MX220F032D

# tool configuration
FLASH   = sudo ubw32 -n -r
CC      = xc32-gcc
BIN2HEX = xc32-bin2hex
AR      = xc32-ar

# Microchip Application Library path
# must point to "Microchip" folder in that library
APPLIB_PATH = $(HOME)/devel/pinguino/applibs/Microchip

# Our own library, relative to the project path
FLAUSCHLIB_PATH = ../../flauschlib

# compiler config
OPTIMIZE ?= -Os -mips16e
CFLAGS ?= -g $(OPTIMIZE) -mprocessor=$(PROC)
CFLAGS += -I. -I$(FLAUSCHLIB_PATH) "-I$(APPLIB_PATH)/Include"
LDFLAGS += -g $(OPTIMIZE) -mprocessor=$(PROC) -Wl,--report-mem

# add needed libraries
LIBS += $(FLAUSCHLIB_PATH)/libflausch.a

#############################################################################
# you can probably leave the following as they are:

all: $(MAIN).hex

$(MAIN).hex: $(MAIN).elf
	$(BIN2HEX) -a $<

$(MAIN).elf: $(OBJECTS) $(APPLIB) $(LIBS) procdefs.ld
	$(CC) $(LDFLAGS) $(OBJECTS) $(APPLIB) $(LIBS) -Wl,-Map,$(MAIN).map -o $(MAIN).elf

procdefs.ld:
	echo "You need to copy a procdefs.ld in order to respect the bootloader."

$(FLAUSCHLIB_PATH)/libflausch.a:
	cd $(FLAUSCHLIB_PATH) && make

$(APPLIB): $(APPLIB_OBJECTS)
	$(AR) -r "$@" $(APPLIB_OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c "$<" -o "$@"

write: $(MAIN).hex
	$(FLASH) -w $(MAIN).hex

clean:
	- rm $(OBJECTS) $(APPLIB_OBJECTS) $(APPLIB)
	- rm -f $(MAIN).elf $(MAIN).hex $(MAIN).map
