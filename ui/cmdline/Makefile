# sub Makefile

# inclusions
INCFLAGS:= -Iinc -I../../common/ince -I../../crypto/ince -I../../volume/ince -I../../fuse/ince -I../../core/ince

# Objects needed
CXXFILES:= $(shell find src -name '*.cpp')

# external components needed
EXTERNAL_STATIC_LIBS=../../$(BD)/core.a ../../$(BD)/fuse.a ../../$(BD)/volume.a ../../$(BD)/crypto.a ../../$(BD)/common.a

#external linker components
EXTERNAL_LINK_LIBS=-lfuse

# actual compilation
include ../../Makefile
