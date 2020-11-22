# sub Makefile

# inclusions
INCFLAGS:= -Iince -Iinci -I../common/ince -I../volume/ince

# Objects needed
CXXFILES:= $(shell find src -name '*.cpp')

# C files for e2fsprogs
CFILES+= src/e2fsprogs/fuse2fs.c

# actual compilation
include ../Makefile

