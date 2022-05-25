# sub Makefile

# inclusions
INCLUDES:= inc ../../common/ince ../../crypto/ince ../../volume/ince ../../fuse/ince ../../core/ince .. src/argtable3

# Objects needed
CXXFILES:= $(shell find src -name '*.cpp')
CFILES:= src/argtable3/argtable3.c

# external components needed (order is important)
STATIC_LIBS=core.a fuse.a volume.a crypto.a common.a

# Additional flags useful to build final binary
ifeq ($(T),release)
CFLAGS += -flto
endif

# external linker components
EXTERNAL_LINK_LIBS += -lfuse
EXTERNAL_LINK_LIBS += -lpthread
EXTERNAL_LINK_LIBS+= -lrt

# actual compilation
include rules.mk
