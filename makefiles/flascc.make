FLASCC:=F:/FlasCC/sdk
FLEX:=f:/flex_sdk_4.6
GLS3D:=F:/FlasCC/GLS3D/install/usr

AS3COMPILER:=asc2.jar

PLATFORM = FLASCC
PLATFORM_LIBS = -O4 -lSDL -lSDLmain -lvgl -emit-swf -swf-size=512x384  -swf-version=18 -symbol-abc=$(OBJDIR)/Console.abc
PLATFORM_FLAGS = -O4  -DNO_ZLIB -DENDIAN_SDL -DHAS_SOCKLEN_T -DHAVE_UNISTD_H -DHAVE_INTTYPES_H -DHAVE_STDINT_H -DFLOATPOINT -Wno-trigraphs
PLATFORM_DIRECTORY = platform/flascc
PLATFORM_OBJECTS = vfs.abc

LDFLAGS+=-L"$(GLS3D)/lib"
#-L"$(FLASCC)/usr/lib/"
CPPFLAGS+=-I"$(GLS3D)/include"
#-I"$(FLASCC)/usr/include/"

OBJDIR = objects-flascc
BUILDDIR = build/flash

INCLUDE_PAWN = TRUE
PAWNSIMPLEEXEC = TRUE

ASM =
ASMTYPE = elf

CPP = g++
CC = gcc

BIN  = luxengine.swf
EXTRABINARIES = $(OBJDIR)/Console.abc
BINEXT=.swf
OPENGL = FALSE

NETWORK = FALSE
ONLINEENABLED = FALSE
NET_OBJECT = $(OBJDIR)/enet/unix.o



ifneq (,$(findstring "asc2.jar","$(AS3COMPILER)"))
	$?AS3COMPILERARGS=java $(JVMARGS) -jar $(FLASCC)/usr/lib/$(AS3COMPILER) -merge -md
else
	echo "ASC is no longer supported" ; exit 1 ;
endif

vfs.abc:
	cp "$(FLASCC)/usr/share/LSOBackingStore.as" .
	$(FLASCC)/usr/bin/genfs extra/flascc vfs
	$(AS3COMPILERARGS)	-import $(FLASCC)/usr/lib/builtin.abc -import $(FLASCC)/usr/lib/playerglobal.abc \
				-import $(FLASCC)/usr/lib/BinaryData.abc -import $(FLASCC)/usr/lib/ISpecialFile.abc \
				-import $(FLASCC)/usr/lib/IBackingStore.abc -import $(FLASCC)/usr/lib/IVFS.abc \
				-import $(FLASCC)/usr/lib/InMemoryBackingStore.abc -import $(FLASCC)/usr/lib/PlayerKernel.abc \
				LSOBackingStore.as vfsRootFSBackingStore.as -outdir . -out vfs

$(OBJDIR)/Console.abc:
	$(AS3COMPILERARGS)	-import $(FLASCC)/usr/lib/builtin.abc \
				-import $(FLASCC)/usr/lib/ISpecialFile.abc \
				-import $(FLASCC)/usr/lib/CModule.abc \
				-import $(FLASCC)/usr/lib/playerglobal.abc \
				-import vfs.abc \
				src/$(PLATFORM_DIRECTORY)/Console.as -outdir $(OBJDIR) -out Console

