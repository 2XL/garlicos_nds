#-------------------------------------------------------------------------------
# Makefile for a GARLIC program (Executable)
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Current directory for GARLIC's API content
#-------------------------------------------------------------------------------
GARLICAPI	:=	../../GARLIC_API
				# assume a relative structure of the GARLIC project directories

#---------------------------------------------------------------------------------
# TARGET is the name of the project's directory
#---------------------------------------------------------------------------------
TARGET	:=	$(shell basename $(CURDIR))

#-------------------------------------------------------------------------------
# options for Executable code generation
#-------------------------------------------------------------------------------
ARCH	:=	-march=armv5te -mlittle-endian

CFLAGS	:=	-Wall -g0 $(ARCH) -mtune=arm946e-s -fomit-frame-pointer -ffast-math
				# -Wall						: enable all warnings
				# -g0						: disable debug info generation
				# $(ARCH) -mtune=arm946e-s	: tune code generation for specific machine
				# -fomit-frame-pointer 		: avoid to use a 'frame-pointer' register in functions that do not need it
				# -ffast-math				: optimize math operations

ASFLAGS	:=	-g0 $(ARCH) -mcpu=arm946e-s
				# -g0						: disable debug info generation
				# $(ARCH) -mcpu=arm946e-s	: define architecture and machine

LDFLAGS	:=	--oformat elf32-littlearm \
			--emit-relocs --gc-sections --strip-debug
				# --oformat elf32-littlearm	: define output code target
				# --emit-relocs				: generate relocations in final output
				# --gc-sections				: remove unused sections
				# --strip-debug				: strip debugging symbols

#-------------------------------------------------------------------------------
# make commands
#-------------------------------------------------------------------------------

$(TARGET).elf : $(TARGET).o
	@echo "linking $(TARGET).o into $(TARGET).elf"
	@arm-none-eabi-ld $(LDFLAGS) $(TARGET).o $(GARLICAPI)/GARLIC_API.o -o $(TARGET).elf

$(TARGET).o : $(TARGET).s
	@echo "assembling $(TARGET).s into $(TARGET).o"
	@arm-none-eabi-as $(ASFLAGS) $(TARGET).s -o $(TARGET).o

$(TARGET).s : $(TARGET).c
	@echo "compiling $(TARGET).c into $(TARGET).s"
	@arm-none-eabi-gcc $(CFLAGS) -S $(TARGET).c -I$(GARLICAPI) -o $(TARGET).s

#---------------------------------------------------------------------------------
# clean commands
#---------------------------------------------------------------------------------
clean : 
	@rm -fv $(TARGET).s
	@rm -fv $(TARGET).o
	@rm -fv $(TARGET).elf
