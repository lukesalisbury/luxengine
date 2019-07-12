#Note: Local platform config

config_local: config_header
ifeq ($(BUILDPLATFORM), Windows)
	${shell write.exe "platform_includes=-I\"$(SUPPORTPATH)/include\" -I\"$(SUPPORTPATH)/include/SDL2\" " >> $(configninja)}
	${shell write.exe "platform_lib_flags=-L\"$(SUPPORTPATH)/lib\"" >> $(configninja)}
else
	@echo platform_includes=${shell sdl2-config --cflags} >> $(configninja)
	@echo platform_lib_flags=${shell sdl2-config --libs} >> $(configninja)
endif
