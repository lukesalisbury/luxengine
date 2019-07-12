PLATFORM = __JAVA__
PLATFORM_LIBS = 
PLATFORM_FLAGS = -DBYTE_ORDER=BIG_ENDIAN -DAMX_ANSIONLY=TRUE -DNDEBUG -DHAVE_ALLOCA_H 
RES = 
BIN = luxengine.mips
CPP = mips-unknown-elf-g++

OBJDIR = objects-java

FINALOUTPUT = luxengine.class

luxengine.class: $(BIN)
	java org.ibex.nestedvm.Compiler -outfile luxengine.class luxengine $(BIN)
