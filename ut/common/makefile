# sub Makefile

# inclusions
INCLUDES:=ince ../unity/ince ../../src/common/ince

# flags
CFLAGS+=-DINCLUDE_TEST_COMMON

# Objects needed
CXXFILES:= $(shell find src -name '*.cpp')
CXXFILES+= mainTests.cpp

# external components needed
STATIC_LIBS=common.a unity.a

# linux needs a link to rt for shared memory functions
ifeq ($(P),linux)
EXTERNAL_LINK_LIBS+= -lrt
endif

# actual compilation
include rules.mk

