ifeq ($(SUPPORTLIBS), )
	SUPPORTLIBS = /C/dev/kos
endif

#Settings
BUILDDEBUG = FALSE
OPTIMIZE = LITTLE
INCLUDE_PAWN = TRUE
NETWORK = FALSE
DOWNLOADER_MODE = none
OPENGL = FALSE

BUILDDIR = ./bin/dc
OBJDIR = ./objects-dc

BIN  = luxengine.elf
CPP = $(SUPPORTLIBS)/sh-elf/bin/sh-elf-g++
CC = $(SUPPORTLIBS)/sh-elf/bin/sh-elf-gcc

#RES = $(OBJDIR)/romdisk.o
#RES_SOURCE = $(OBJDIR)/romdisk.img
RES =
RES_SOURCE =
FINALOUTPUT = $(BUILDDIR)/luxengine-dc.bin

#Static Settings
INC_PATHS = -I'$(SUPPORTLIBS)/kos/include' -I'$(SUPPORTLIBS)/kos/kernel/arch/dreamcast/include' -I'$(SUPPORTLIBS)/kos/addons/include' -I'$(SUPPORTLIBS)/kos-ports/include' -I'$(SUPPORTLIBS)/kos-ports/include/zlib'
LIB_PATHS = -L'$(SUPPORTLIBS)/kos/lib/dreamcast' -L'$(SUPPORTLIBS)/kos/addons/lib/dreamcast'

PLATFORM = DREAMCAST
PLATFORM_DIRECTORY = platform/dc
PLATFORMBITS = 

PLATFORM_LIBS = -ml -m4-single-only -Wl,-Ttext=0x8c010000 -T$(SUPPORTLIBS)/kos/utils/ldscripts/shlelf.xc -nodefaultlibs $(LIB_PATHS) -Wl,--start-group -lkallisti -lc -lstdc++ -lgcc -Wl,--end-group
PLATFORM_LIBS += -lz -lSDL_mixer_126 -lSDL_1213 -ltremor -lc -lgcc -lm

PLATFORM_FLAGS = -DHAVE_STDINT_H -DHAVE_ALLOCA_H -DTINYXML_USE_STL
PLATFORM_FLAGS += -ml -m4-single-only -fno-crossjumping $(INC_PATHS) -D_arch_dreamcast -D_arch_sub_pristine -Wall -g -fno-builtin -fno-strict-aliasing

PLATFORM_OBJECTS =



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


