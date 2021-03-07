# sub Makefile

# inclusions
INCLUDES:=ince inci

# Objects needed
CXXFILES:= $(shell find src -path src/platform -prune -o -name '*.cpp' -print)

# inclusion of platform-specific files
ifdef P
CXXFILES+= $(shell find src/platform/$(P) -name '*.cpp')
endif

# actual compilation
include rules.mk

