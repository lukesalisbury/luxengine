PLATFORM = __NDS__
PLATFORMBITS = other
PLATFORM_LIBS = -L"$(DEVKITPRO)\devkitARM\lib"  -L"$(DEVKITPRO)\libnds\lib" -L"$(DEVKITPRO)\portlibs\arm\lib" -marm -MMD -MP -march=armv5te -mtune=arm946e-s -funsigned-char -fomit-frame-pointer -ffast-math -mthumb -mthumb-interwork -specs=ds_arm9.specs  -lpng -lz -ldswifi9 -lmm9 -lfilesystem -lfat -lnds9 
PLATFORM_FLAGS = -funsigned-char -DHAVE_STDINT_H -DHAVE_ALLOCA_H -DAMX_ANSIONLY -DNOCONSOLE -DARM9 -I"$(DEVKITPRO)\devkitARM\arm-eabi\include" -I"$(DEVKITPRO)\libnds\include" -I"$(DEVKITPRO)\portlibs\arm\include" -I"$(DEVKITPRO)\devkitARM\lib\gcc\arm-eabi\4.5.1\include" -Wall -g
PLATFORM_DIRECTORY = platform/nds

INCLUDE_PAWN = TRUE
PORTAL = TRUE
RES = 


CPP = arm-none-eabi-g++.exe
CC =  arm-none-eabi-gcc.exe
BUILDDIR = build/nds
OBJDIR = objects-nds
OPENGL = FALSE
ONLINEENABLED = FALSE
BUILDDEBUG = FALSE
OPTIMIZE = NONE

BIN  = luxengine.elf
FINALOUTPUT = $(BUILDDIR)/luxengine.nds

$(OBJDIR)/luxengine.arm9: $(BIN)
	arm-none-eabi-objcopy.exe -O binary $(BUILDDIR)/$(BIN) $(OBJDIR)/luxengine.arm9 

$(BUILDDIR)/luxengine.nds: $(OBJDIR)/luxengine.arm9
	ndstool -c $(BUILDDIR)/luxengine.nds -9 $(OBJDIR)/luxengine.arm9  -b res/dslogo.bmp "Mokoi Gaming's;Lux Engine;0.94" -g LUXE MG Lux_Engine 1
	ndstool -c $(BUILDDIR)/luxengine2.nds -9 $(OBJDIR)/luxengine.arm9  -b res/dslogo.bmp "Mokoi Gaming's;Lux Engine NitroFS;0.94" -g LUXE MG Lux_Engine 1 -d F:/mokoi_svn-old/build-Stuff/nitrofs/


