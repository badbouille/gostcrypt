# sub Makefile

# inclusions
INCLUDES:= ince inci ../common/ince ../volume/ince ../crypto/ince ../fuse/ince

# Windows inclusions (fuse-winfsp)
ifeq ($(P),windows)
INCLUDES+=../fuse/src/winfsp/inc ../fuse/src/winfsp/inc/fuse ../fuse/src/winfsp/opt/fsext/inc
endif

# Objects needed
CXXFILES:= $(shell find src -name '*.cpp')

# actual compilation
include rules.mk

