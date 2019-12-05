#*****************************************************************************
#
# Copyright 2013,2014 Altera Corporation. All Rights Reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
# 
# 3. Neither the name of the copyright holder nor the names of its contributors
# may be used to endorse or promote products derived from this software without
# specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
# 
#*****************************************************************************

#
# $Id$
#

ALT_DEVICE_FAMILY ?= soc_cv_av
SEMIHOSTING ?= 1
DEBUG ?= 0

SOCEDS_ROOT ?= C:\intelFPGA\embedded
HWLIBS_ROOT = $(SOCEDS_ROOT)/ip/altera/hps/altera_hps/hwlib

HWLIBS_SRC  := alt_address_space.c alt_bridge_manager.c alt_cache.c alt_clock_manager.c alt_dma.c alt_dma_program.c alt_fpga_manager.c
EXAMPLE_SRC := main.c alt_hps_detect.c FPGADemo.c FPGASetup.c EE30186.c FanFunctions.c DisplayFunctions.c MiscFunctions.c
C_SRC       := $(EXAMPLE_SRC) $(HWLIBS_SRC)

MULTILIBFLAGS := -mcpu=cortex-a9 -mfloat-abi=softfp -mfpu=neon
CFLAGS  := -g -O0 -Wall -Werror -std=c99 $(MULTILIBFLAGS) -I$(HWLIBS_ROOT)/include -DALT_FPGA_ENABLE_DMA_SUPPORT=1 -I$(HWLIBS_ROOT)/include/$(ALT_DEVICE_FAMILY) -D$(ALT_DEVICE_FAMILY)

CROSS_COMPILE := arm-altera-eabi-
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)g++
NM := $(CROSS_COMPILE)nm
OD := $(CROSS_COMPILE)objdump
OC := $(CROSS_COMPILE)objcopy

RM := rm -rf
CP := cp -f

ifeq ($(SEMIHOSTING),0)
CFLAGS := $(CFLAGS) -DPRINTF_UART
LINKER_SCRIPT := cycloneV-dk-ram.ld
HWLIBS_SRC += alt_16550_uart.c alt_printf.c alt_p2uart.c
else
LINKER_SCRIPT := cycloneV-dk-ram-hosted.ld
ifeq ($(DEBUG),0)
CFLAGS := $(CFLAGS) 
else 
CFLAGS := $(CFLAGS) -DPRINTF_HOST
endif
endif

LDFLAGS := -T$(LINKER_SCRIPT) $(MULTILIBFLAGS)

C_SRC := $(EXAMPLE_SRC) $(HWLIBS_SRC)

ELF ?= $(basename $(firstword $(C_SRC))).axf
SPL := u-boot-spl.axf
OBJ := $(patsubst %.c,%.o,$(C_SRC))

.PHONY: all
all: $(ELF) $(SPL)

.PHONY: clean
clean:
	$(RM) $(ELF) $(SPL) $(OBJ) *.objdump *.map *.rbf $(HWLIBS_SRC) soc_system* cpf_option.txt

define SET_HWLIBS_DEPENDENCIES
$(1): $(2)
	$(CP) $(2) $(1)
endef

ALL_HWLIBS_SRC = $(wildcard $(HWLIBS_ROOT)/src/hwmgr/*.c) $(wildcard $(HWLIBS_ROOT)/src/hwmgr/$(ALT_DEVICE_FAMILY)/*.c $(wildcard $(HWLIBS_ROOT)/src/utils/*.c))

$(foreach file,$(ALL_HWLIBS_SRC),$(eval $(call SET_HWLIBS_DEPENDENCIES,$(notdir $(file)),$(file))))

soc_system.sof: ARM.sof
	$(CP) $< $@

# No Data Compression
soc_system_nodc.rbf: soc_system.sof
	quartus_cpf -c $< $@

# With Data Compression
soc_system_dc.rbf: soc_system.sof
	$(RM) cpf_option.txt
	echo bitstream_compression=on > cpf_option.txt
	quartus_cpf -c -o cpf_option.txt $< $@
	$(RM) cpf_option.txt

soc_system_nodc.o: soc_system_nodc.rbf
	$(OC) --input-target binary --output-target elf32-little --alt-machine-code 40 $< $@

soc_system_dc.o: soc_system_dc.rbf
	$(OC) --input-target binary --output-target elf32-little --alt-machine-code 40 $< $@

$(SPL): u-boot-spl
	$(CP) $< $@
	$(OD) -d $@ > $@.objdump

$(OBJ): %.o: %.c Makefile
	$(CC) $(CFLAGS) -c $< -o $@

$(ELF): $(OBJ) soc_system_dc.o
	$(LD) $(LDFLAGS) $(OBJ) soc_system_dc.o -o $@
	$(OD) -d $@ > $@.objdump
	$(NM) $@ > $@.map
