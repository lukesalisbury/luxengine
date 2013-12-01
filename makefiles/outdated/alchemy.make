PLATFORM = LINUX
PLATFORM_LIBS = -swc
PLATFORM_FLAGS = -DHAS_SOCKLEN_T -DHAVE_UNISTD_H -DHAVE_INTTYPES_H -DHAVE_STDINT_H -DFLOATPOINT -I"$(SUPPORTPATH)/include/c++/3.4/include" -I"$(SUPPORTPATH)/include/c++/3.4"
PLATFORM_DIRECTORY = platform/pc
INCLUDE_PAWN = TRUE

ASM = 
CPP = i686-pc-cygwin-llvm-g++.exe
CC = i686-pc-cygwin-llvm-gcc.exe
RES = 
BIN  = luxengine-alchemy.swc
TEMP = F:/temp/

OPENGL = FALSE
NETWORK = FALSE

#NET_OBJECT = $(OBJDIR)/pc/net/host.o  $(OBJDIR)/pc/net/packet.o $(OBJDIR)/pc/net/peer.o $(OBJDIR)/pc/net/protocol.o $(OBJDIR)/pc/net/unix.o $(OBJDIR)/pc/net/win32.o $(OBJDIR)/pc/net/list.o

$(OBJDIR)/%.o : %.asm ;
	@$(ASM) -O1 -f elf -o $@ $<
