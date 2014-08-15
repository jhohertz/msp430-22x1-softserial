# MSP430 Makefile
# #####################################
#
# Part of the uCtools project
# uctools.github.com
#
#######################################
# user configuration:
#######################################
# Frequency assumption for timing calculation
FREQ=1
# TARGET: name of the output file
TARGET = main
# MCU: part number to build for
MCU = msp430g2211
# SOURCES: list of input source sources
SOURCES = main.c 
# INCLUDES: list of includes, by default, use Includes directory
INCLUDES = -Isrc/include
# OUTDIR: directory to use for output
OUTDIR = build
# define flags
CFLAGS = -mcpu=430 -mmcu=$(MCU) -g -Os -Wall -Wunused $(INCLUDES)
CPPFLAGS = 
ASFLAGS = -mcpu=430 -mmcu=$(MCU) -x assembler-with-cpp -Wa,-gstabs
LDFLAGS = -mcpu=430 -mmcu=$(MCU) -Wl,-Map=$(OUTDIR)/$(TARGET).map
#######################################
# end of user configuration
#######################################
#
#######################################
# binaries
#######################################
CC      	= msp430-gcc
LD      	= msp430-ld
AR      	= msp430-ar
AS      	= msp430-gcc
GASP    	= msp430-gasp
NM      	= msp430-nm
OBJCOPY 	= msp430-objcopy
MAKETXT 	= srec_cat
UNIX2DOS	= unix2dos
RM      	= rm -f
MKDIR		= mkdir -p
#######################################

# file that includes all dependencies
DEPEND = $(SOURCES:.c=.d)

# list of object files, placed in the build directory regardless of source path
OBJECTS = $(addprefix $(OUTDIR)/,$(notdir $(SOURCES:.c=.o)))

# default: build hex file and TI TXT file
all: $(OUTDIR)/$(TARGET).hex $(OUTDIR)/$(TARGET).txt

# TI TXT file
$(OUTDIR)/%.txt: $(OUTDIR)/%.hex
	$(MAKETXT) -O $@ -TITXT $< -I
	$(UNIX2DOS) $(OUTDIR)/$(TARGET).txt

# intel hex file
$(OUTDIR)/%.hex: $(OUTDIR)/%.elf
	$(OBJCOPY) -O ihex $< $@

# elf file
$(OUTDIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBS) -o $@

$(OUTDIR)/%.o: src/%.c | $(OUTDIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

$(OUTDIR)/%.s: src/%.c | $(OUTDIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -S $< -o $@

# assembly listing
$(OUTDIR)/%.lst: src/%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -Wa,-anlhd $< > $@

# create the output directory
$(OUTDIR):
	$(MKDIR) $(OUTDIR)

# remove build artifacts and executables
clean:
	-$(RM) $(OUTDIR)/*

.PHONY: all clean
