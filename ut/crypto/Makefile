# sub Makefile

# inclusions
INCFLAGS:= -Iince -I../unity/ince -I../../common/ince -I../../crypto/ince

# flags
CFLAGS+=-DINCLUDE_TEST_COMMON

# Objects needed
CFILES:= $(shell find src -name '*.cpp')
CFILES+= mainTests.cpp

# external components needed
EXTERNAL_STATIC_LIBS=../../$(BD)/common.a ../../$(BD)/crypto.a ../../$(BDUT)/unity.a

# actual compilation
include ../../Makefile

