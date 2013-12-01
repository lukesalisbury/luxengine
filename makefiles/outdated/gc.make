MACHDEP =  -DGEKKO -mogc -mcpu=750 -meabi -mhard-float

PLATFORM = __GAMECUBE__
PLATFORM_LIBS = -L"D:\devkitPro\devkitPPC\lib" -L"D:\devkitpro\devkitPPC\powerpc-eabi\lib" -L"D:\devkitPro\libogc\lib\cube" $(MACHDEP) -L"F:\mokoi_svn\trunk\luxengine\src\GRRLIB" -lgrrlib.gc -lz -lfat  -logc -liberty -lc -lm 
PLATFORM_FLAGS = -DHAVE_STDINT_H -DHAVE_ALLOCA_H -DHAVE_INTTYPES_H -DNOCONSOLE -DAMX_ANSIONLY -DGEKKO -I"D:\devkitPro\devkitPPC\include" -I"D:\devkitPro\libogc\include" -Os $(MACHDEP) -fno-exceptions -fno-rtti
PLATFORM_DIRECTORY = platform/gc

INCLUDE_PAWN = TRUE
PORTAL = TRUE
RES = 
BIN  = $(OBJDIR)/luxengine.gc.elf
FINALOUTPUT = luxengine.gc.dol


CPP = powerpc-eabi-g++.exe
OPENGL = FALSE
BUILDDEBUG = FALSE
OPTIMIZE = NONE


$(FINALOUTPUT): $(OBJDIR)/luxengine.gc.elf
	elf2dol  $< $@
