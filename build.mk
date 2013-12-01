ifeq ($(PLATFORMBITS), 64)
	COMPILER_FLAGS += -m64
	COMPILER_LIBS +=  -m64
endif
ifeq ($(PLATFORMBITS), 32)
	COMPILER_FLAGS +=  -m32
	COMPILER_LIBS +=  -m32
endif


COMPILER_FLAGS += $(CFLAGS)
COMPILER_FLAGSPP += $(CPPFLAGS)
COMPILER_LIBS += $(LDFLAGS)




.PHONY: all-before all clean 
	@echo --------------------------------

all: all-before $(BIN) $(FINALOUTPUT)
	@echo --------------------------------

all-before:
	@echo --------------------------------
	@echo Building $(BIN) $(PROGRAM_VERSION)
	@echo Build Platform: $(BUILDPLATFORM)
	@echo Target Platform: $(BUILDOS)/$(PLATFORMBITS)
	@echo Debug Build? $(BUILDDEBUG)
	@echo Build Flags: $(COMPILER_FLAGS)
	@echo Build Libs: $(COMPILER_LIBS)
	@echo --------------------------------

$(BIN): $(OBJ)
	@echo --------------------------------
	@echo Building $(BIN) $(MESSAGE)
	@echo Build Flags: $(COMPILER_FLAGS)
	@echo Build Libs: $(COMPILER_LIBS)
	@echo --------------------------------
	$(CPP) $(OBJ) -o $(BUILDDIR)/$(BIN) $(COMPILER_LIBS)

install: $(BIN)
	@echo Installing $< to $(INSTALLDIR)
	@-mkdir -p $(dir $@)
	@cp $(BUILDDIR)/$(BIN)  $(INSTALLDIR)

clean: 
	$(RM) $(OBJ) $(BUILDDIR)/$(BIN)

objects/%.o : src/%.cpp
	@-mkdir -p $(dir $@)
#	@echo Compiling $@ $(MESSAGE)
	@$(CPP) $(COMPILER_FLAGSPP) -o $@ -c $<

objects/%.o : ../elix/src/%.cpp
	@-mkdir -p $(dir $@)
#	@echo Compiling $@ $(MESSAGE)
	@$(CPP) $(COMPILER_FLAGSPP) -o $@ -c $<

$(OBJDIR)/%.o : src/%.c
#	@echo Compiling $@ $(MESSAGE)
	@-$(MKDIR) $(dir $@)
	@$(CC) -c $(COMPILER_FLAGS) -o $@ $<

