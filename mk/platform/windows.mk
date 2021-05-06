
#################################################
## Windows Configuration for GostCrypt project ##
#################################################

# Toolchain
CXX:=x86_64-w64-mingw32-g++
CC:=x86_64-w64-mingw32-gcc
AR:=x86_64-w64-mingw32-ar
DOX:=doxygen

# C Flags

# Flags to ship libraries with binaries (easier for windows)
CFLAGS += -static-libgcc
CFLAGS += -static-libstdc++

# FUSE flags
CFLAGS += -D_FILE_OFFSET_BITS=64
CFLAGS += -DFUSE_USE_VERSION=29

# Adding flags depending on options

ifeq ($(T),debug)
CFLAGS += -O0 -g
endif

ifeq ($(T),release)
CFLAGS += -O3
endif

CFLAGS += -DPLATFORM_WINDOWS

# inclusion flags
CFLAGS += $(addprefix -I, $(INCLUDES))

# C++ flags
CXXFLAGS += $(CFLAGS)
CXXFLAGS += -std=c++17

