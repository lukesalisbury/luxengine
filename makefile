#
${shell gcc write.c -s -o write.exe}

configninja="buildconfig/config.ninja"
filesninja="buildconfig/files.ninja"

BUILDPLATFORM=$(strip ${shell uname})
BUILDMACHINE=$(strip ${shell uname -m})

PLATFORMPROCESSOR?=x86
PLATFORMBITS?=unknown

CONFIG_OPENGL?=TRUE

#Build platform & target
ifeq ($(PLATFORMOS), ) # No platform set, so we guess
	PLATFORMOS=windows
	ifeq ($(BUILDPLATFORM), Linux)
		PLATFORMOS	= linux
	endif
	ifeq ($(BUILDPLATFORM), Windows)
		PLATFORMOS	= windows
	endif
	ifeq ($(BUILDPLATFORM), Apple)
		PLATFORMOS	= apple
	endif
	ifeq ($(BUILDPLATFORM), Darwin)
		PLATFORMOS	= apple
	endif
	ifeq ($(BUILDPLATFORM), Haiku)
		PLATFORMOS	= haiku
	endif

	#Plaform bits
	ifeq ($(BUILDMACHINE), x86_64)
		PLATFORMBITS=64
		PLATFORMPROCESSOR=x86_64
	endif
	ifeq ($(BUILDMACHINE), x86)
		PLATFORMBITS=32
		PLATFORMPROCESSOR=x86
	endif
	ifeq ($(BUILDMACHINE), i686)
		PLATFORMBITS=32
		PLATFORMPROCESSOR=x86
	endif
	ifeq ($(BUILDMACHINE), i386)
		PLATFORMBITS=32
		PLATFORMPROCESSOR=x86
	endif
endif

PLATFORMOSUC=${shell echo $(PLATFORMOS) | tr 'a-z' 'A-Z'}

include buildscripts/settings/*.mk

#Build
.PHONY: clean

all: debug
	ninja 1>&2

clean:
	ninja clean 1>&2
	@rm $(filesninja)
	@rm $(configninja)

info:
	@echo --------------------------------
	@echo Build Platform: $(BUILDPLATFORM)
	@echo Target Platform: $(PLATFORMOS)/$(PLATFORMBITS)
	@echo --------------------------------
	@cat $(configninja)
	@echo --------------------------------



$(filesninja): $(OBJ)
	${shell write.exe "build $${program}: link $(OBJ:%=$${object_dir}/%)" >> $(filesninja)}

%.o:
	${shell write.exe "build $${object_dir}/$@: compile src/$(@:%.o=%.cpp)" >> $(filesninja)}

file_header:
	${shell write.exe "#FileObjects" > $(filesninja)}

file: file_header $(filesninja)
	@echo Gen $(filesninja)

config_header:
	${shell write.exe "platform_ninja=$(PLATFORMOS)-$(PLATFORMPROCESSOR).ninja" > $(configninja)}

include buildscripts/configlocal.mk

config: config_local
	@echo Gen $(configninja)

debug: config file
	@echo Debug Mode
	${shell write.exe "compile_mode=debug" >> $(configninja)}

release: config file
	@echo Release Mode
	${shell write.exe "compile_mode=release" >> $(configninja)}



