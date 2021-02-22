# sub Makefile

# inclusions
INCLUDES:= ince ../unity/ince ../../src/common/ince ../../src/crypto/ince

# flags
CFLAGS+=-DINCLUDE_TEST_COMMON

# Objects needed
CXXFILES:= $(shell find src -name '*.cpp')
CXXFILES+= mainTests.cpp

# external components needed
STATIC_LIBS=crypto.a common.a unity.a

# actual compilation
include rules.mk

