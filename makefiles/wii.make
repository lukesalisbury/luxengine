MACHDEP =  -DGEKKO -mrvl -mcpu=750 -meabi -mhard-float

PLATFORM = __WII__
PLATFORM_LIBS = -L"$(DEVKITPRO)\devkitPPC\lib" -L"$(DEVKITPRO)\devkitPPC\powerpc-eabi\lib" -L"$(DEVKITPRO)\libogc\lib\wii" -L"$(DEVKITPRO)\portlibs\ppc\lib" -L"F:\devkitPro\wii\lib"  $(MACHDEP)  -lwiisprite -lpng -lz -lwiiuse -lbte  -lfat -logc -liberty -lc -lm
PLATFORM_FLAGS =  -DHAVE_STDINT_H -DHAVE_ALLOCA_H -DHAVE_INTTYPES_H -DNOCONSOLE -DAMX_ANSIONLY -DGEKKO -I"$(DEVKITPRO)\devkitPPC\include" -I"$(DEVKITPRO)\libogc\include" -I"$(DEVKITPRO)\portlibs\ppc\include" -I"$(DEVKITPRO)\wii\include" -Os $(MACHDEP)
PLATFORM_DIRECTORY = platform/wii


INCLUDE_PAWN = TRUE
PORTAL = TRUE
RES = 


CC = powerpc-eabi-gcc.exe
CPP = powerpc-eabi-g++.exe
BUILDDIR = bin/wii
OBJDIR = objects-wii
OPENGL = FALSE
BUILDDEBUG = FALSE
OPTIMIZE = NONE

BIN  = $(OBJDIR)/luxengine.wii.elf
FINALOUTPUT = luxengine.wii.dol


$(FINALOUTPUT): $(OBJDIR)/luxengine.wii.elf
	elf2dol  $< $@

