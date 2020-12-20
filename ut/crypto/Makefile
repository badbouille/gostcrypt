# sub Makefile

# inclusions
INCFLAGS:= -Iince -I../unity/ince -I../../common/ince -I../../crypto/ince

# flags
CFLAGS+=-DINCLUDE_TEST_COMMON

# Objects needed
CXXFILES:= $(shell find src -name '*.cpp')
CXXFILES+= mainTests.cpp

# external components needed
EXTERNAL_STATIC_LIBS=../../$(BDLIB)/crypto.a ../../$(BDLIB)/common.a ../../$(BDLIB)/unity.a

# actual compilation
include ../../Makefile

