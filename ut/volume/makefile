# sub Makefile

# inclusions
INCLUDES:= ince ../unity/ince ../../src/common/ince ../../src/crypto/ince ../../src/volume/ince

# flags
CFLAGS+=-DINCLUDE_TEST_COMMON

# Objects needed
CXXFILES:= $(shell find src -name '*.cpp')
CXXFILES+= mainTests.cpp

# external components needed
STATIC_LIBS=volume.a crypto.a common.a unity.a

# windows needs a link to ws2_32 for htonl only..
ifeq ($(P),windows)
EXTERNAL_LINK_LIBS+= -lws2_32
endif

# actual compilation
include rules.mk

