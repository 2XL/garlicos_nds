#-------------------------------------------------------------------------------
# Makefile for GARLIC API's Object (interface routines)
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# options for Object code generation
#-------------------------------------------------------------------------------
ARCH	:=	-march=armv5te -mlittle-endian

ASFLAGS	:=	-g0 $(ARCH) -mcpu=arm946e-s
				# -g0						: disable debug info generation
				# $(ARCH) -mcpu=arm946e-s	: define architecture and machine

#-------------------------------------------------------------------------------
# make commands
#-------------------------------------------------------------------------------

GARLIC_API.o : GARLIC_API.s
	@echo "assembling GARLIC_API.s into GARLIC_API.o"
	@arm-none-eabi-as $(ASFLAGS) GARLIC_API.s -o GARLIC_API.o

