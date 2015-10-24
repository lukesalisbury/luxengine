CUSTOMOBJECTS = TRUE
CUSTOMPATH = ../custom/ozengine

BIN = openzelda$(BINEXT)

PLATFORM_FLAGS += -DPROGRAM_NAME="\"Open Zelda\""
PLATFORM_FLAGS += -DPROGRAM_DOCUMENTS="\"openzelda-quests\""
PLATFORM_FLAGS += -DPROGRAM_VERSION_STABLE="\"2.0\""
PLATFORM_FLAGS += -DDIRECTORY_FILE="\"edit.zelda\""
PLATFORM_FLAGS += -DGUI_PORTAL_URL="\"openzelda.nfshost.com/games/1.9/\""
PLATFORM_FLAGS += -DPACKAGE_GET_URL="\"http://openzelda.nfshost.com/get/\""
PLATFORM_FLAGS += -DCUSTOMOBJECTS=TRUE
PLATFORM_FLAGS += -I'elix/src'


ifeq ($(BUILDOS),windows)
	RES_SOURCE = $(CUSTOMPATH)/openzelda.rc
endif
