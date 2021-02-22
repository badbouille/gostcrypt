
#################################################
## Windows Configuration for GostCrypt project ##
#################################################

# TODO this is just a copy of the linux platform for now

# Toolchain
CXX:=g++
CC:=gcc
AR:=ar
DOX:=doxygen

# C Flags

# FUSE flags
CFLAGS += -D_FILE_OFFSET_BITS=64
CFLAGS += -DFUSE_USE_VERSION=29

# Arch flags
CFLAGS += -fPIC -march=native

# Adding flags depending on options

ifeq ($(T),debug)
CFLAGS += -O0 -g
endif

ifeq ($(T),release)
CFLAGS += -O4
endif

# inclusion flags
CFLAGS += $(addprefix -I, $(INCLUDES))

# C++ flags
CXXFLAGS += $(CFLAGS)
CXXFLAGS += -std=c++17

