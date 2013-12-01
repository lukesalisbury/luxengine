ifeq ($(SUPPORTLIBS), )
	SUPPORTLIBS = /F/kos
endif


INC_PATHS = -I'$(SUPPORTLIBS)/kos/include' -I'$(SUPPORTLIBS)/kos/kernel/arch/dreamcast/include' -I'$(SUPPORTLIBS)/kos/addons/include' -I'$(SUPPORTLIBS)/kos-ports/include' -I'$(SUPPORTLIBS)/kos-ports/include/zlib'
LIB_PATHS = -L'$(SUPPORTLIBS)/kos/lib/dreamcast' -L'$(SUPPORTLIBS)/kos/addons/lib/dreamcast'

#PLATFORM_FLAGS += -DFRAME_POINTERS -ml -m4-single-only -fno-optimize-sibling-calls -D_arch_dreamcast -D_arch_sub_pristine -fno-builtin -fno-strict-aliasing -ml -m4-single-only
#PLATFORM_FLAGS += -I"$(SUPPORTLIBS)/kos/include" -I"$(SUPPORTLIBS)/kos-ports/include/zlib" -I"$(SUPPORTLIBS)/kos-ports/include" -I"$(SUPPORTLIBS)/kos/kernel/arch/dreamcast/include" -I"$(SUPPORTLIBS)/kos/addons/include"
#PLATFORM_FLAGS += -I"$(SUPPORTLIBS)/sh-elf/include/c++/4.4.4" -I"$(SUPPORTLIBS)/sh-elf/sh-elf/include" -I"$(SUPPORTLIBS)/sh-elf/include/c++/4.4.4/sh-elf"

# Dreamcast Settings
PLATFORM = DREAMCAST
PLATFORM_DIRECTORY = platform/dc


PLATFORM_LIBS = -ml -m4-single-only -Wl,-Ttext=0x8c010000 -T$(SUPPORTLIBS)/kos/utils/ldscripts/shlelf.xc -nodefaultlibs $(LIB_PATHS) -Wl,--start-group -lkallisti -lc -lstdc++ -lgcc -Wl,--end-group
PLATFORM_LIBS += -lz -lSDL_mixer_126 -lSDL_1213 -ltremor -lc -lgcc -lm

PLATFORM_FLAGS = -DHAVE_STDINT_H -DHAVE_ALLOCA_H -DTINYXML_USE_STL
PLATFORM_FLAGS += -ml -m4-single-only -fno-crossjumping $(INC_PATHS) -D_arch_dreamcast -D_arch_sub_pristine -Wall -g -fno-builtin -fno-strict-aliasing


CPP = $(SUPPORTLIBS)/sh-elf/bin/sh-elf-g++
CC = $(SUPPORTLIBS)/sh-elf/bin/sh-elf-gcc
BUILDDIR = build/dc
OBJDIR = objects-dc
#RES = $(OBJDIR)/romdisk.o
#RES_SOURCE = $(OBJDIR)/romdisk.img
RES =
RES_SOURCE =
FINALOUTPUT = $(BUILDDIR)/luxengine-dc.bin
BIN = luxengine.elf



BUILDDEBUG = FALSE
OPTIMIZE = LITTLE
ONLINEENABLED = FALSE
OPENGL = FALSE
NETWORK = FALSE
INCLUDE_PAWN = TRUE
PLATFORMBITS = misc



$(BUILDDIR)/luxengine-dc.bin: luxengine.elf
	$(SUPPORTLIBS)/sh-elf/bin/sh-elf-objcopy -R .stack -O binary $(BUILDDIR)/luxengine.elf $(BUILDDIR)/luxengine-dc.bin
	#lxdream --execute=luxengine-dc.bin
	#$(SUPPORTLIBS)/kos/utils/scramble/scramble $(BINARYDIR)/luxengine-dc.bin $(BINARYDIR)/luxengine-dc-scramble.bin

$(RES): $(RES_SOURCE)
	@echo Creating Disk image
	$(SUPPORTLIBS)/kos/utils/bin2o/bin2o $(RES_SOURCE) romdisk $(RES)
	#bin2o $(RES_SOURCE) romdisk $(RES)

$(RES_SOURCE):
	$(SUPPORTLIBS)/kos/utils/genromfs/genromfs -f romdisk.img -d $(KOS_ROMDISK_DIR) -v -x .svn


