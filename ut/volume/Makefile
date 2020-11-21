# sub Makefile

# inclusions
INCFLAGS:= -Iince -I../unity/ince -I../../common/ince -I../../crypto/ince -I../../volume/ince

# flags
CFLAGS+=-DINCLUDE_TEST_COMMON

# Objects needed
CXXFILES:= $(shell find src -name '*.cpp')
CXXFILES+= mainTests.cpp

# external components needed
EXTERNAL_STATIC_LIBS=../../$(BD)/volume.a ../../$(BD)/crypto.a ../../$(BD)/common.a ../../$(BDUT)/unity.a

# actual compilation
include ../../Makefile

