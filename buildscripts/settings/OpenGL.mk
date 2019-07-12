# OpenGL code
ifeq ($(CONFIG_OPENGL), TRUE)
	OBJ += platform/\$${platform_subsystem}/graphics_opengl.o
	OBJ += gles/gles_display.o gles/shaders.o
	OBJ += gles/draw_vertex1.inc.o gles/draw_vertex2.inc.o
	OBJ += gles/shaders_desktop.o gles/shaders_gles.o gles/shaders_none.o
endif
