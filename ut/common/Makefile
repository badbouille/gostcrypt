# sub Makefile

# inclusions
INCFLAGS:= -Iince -I../unity/ince -I../../common/ince

# flags
CFLAGS+=-DINCLUDE_TEST_COMMON

# Objects needed
CXXFILES:= $(shell find src -name '*.cpp')
CXXFILES+= mainTests.cpp

# external components needed
EXTERNAL_STATIC_LIBS=../../$(BD)/common.a ../../$(BDUT)/unity.a

# actual compilation
include ../../Makefile

