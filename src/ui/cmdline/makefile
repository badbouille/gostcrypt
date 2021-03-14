# sub Makefile

# inclusions
INCLUDES:= inc ../../common/ince ../../crypto/ince ../../volume/ince ../../fuse/ince ../../core/ince ..

# Objects needed
CXXFILES:= $(shell find src -name '*.cpp')

# external components needed (order is important)
STATIC_LIBS=core.a fuse.a volume.a crypto.a common.a

# Additional flags useful to build final binary
ifeq ($(T),release)
CFLAGS += -flto
endif

# external linker components
EXTERNAL_LINK_LIBS += -lfuse
EXTERNAL_LINK_LIBS += -lpthread

# actual compilation
include rules.mk
