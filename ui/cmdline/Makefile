# sub Makefile

# inclusions
INCFLAGS:= -Iinc -I../../common/ince -I../../volume/ince -I../../crypto/ince -I../../fuse/ince -I../../core/ince

# Objects needed
CFILES:= $(shell find src -name '*.cpp')

# external components needed
EXTERNAL_STATIC_LIBS=../../$(BD)/volume.a ../../$(BD)/crypto.a ../../$(BD)/common.a ../../$(BD)/core.a

# actual compilation
include ../../Makefile
