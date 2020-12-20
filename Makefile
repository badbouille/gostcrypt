
###########################################
## Makefile for global GostCrypt Project ##
###########################################

# variables to be defined by submake

# compile in debug or release mode ?
DEBUG?=yes

# flags to use for compilation
INCFLAGS?=-Iince -Iinci
CFLAGS += -D_FILE_OFFSET_BITS=64
CFLAGS += -DFUSE_USE_VERSION=29

# CXX flags
CXXFLAGS += -std=c++17

# list of files to compile for any static or dynamic target
CXXFILES?=
CFILES?=

# Supplementary ressources needed for build
# warning : will not be built or made as a dependency of the binary
EXTERNAL_STATIC_LIBS?=
EXTERNAL_LINK_LIBS?=

# defined by super make (location of object files)
OD?=obj

# defined by super make (binary to build)
BINARY?=

# Path for tools. Using tools from PATH by default
CXX:=g++
CC:=gcc
AR:=ar
DOX:=doxygen

# C flags
OPT_FLAGS:=-O4 -flto
DBG_FLAGS:=-O0 -g
ARCHFLAGS:=-fPIC -msse4.1 -maes -march=native

ifeq ($(DEBUG),yes)
CFLAGS+=$(DBG_FLAGS) $(ARCHFLAGS) $(INCFLAGS)
else
CFLAGS+=$(OPT_FLAGS) $(ARCHFLAGS) $(INCFLAGS)
endif

# bin directory where the binaries and libraries are put
BD:=bin
BDLIB:=$(BD)/lib
BDUT:=$(BD)/ut

# Unity variables for special rule
UNITY_STATIC_LIB:=$(BDLIB)/unity.a
UNITY_DIR:=ut/unity/

# Components that can be built (folders)
COMPONENTS:=common core crypto fuse volume
UNIT_TESTS:=ut_common ut_crypto ut_volume
UI:=cmdline qt

# Objects needed, computed from given sources
CXXOBJS:= $(addprefix $(OD)/, $(subst .cpp,.o,$(CXXFILES)))
COBJS:= $(addprefix $(OD)/, $(subst .c,.o,$(CFILES)))

all: $(COMPONENTS) $(UNIT_TESTS) $(UI)

.SUFFIXES: .cpp .o .

# global compilation rule (works only in submake)
$(CXXOBJS): $(OD)/%.o : %.cpp
	@mkdir -p $(shell dirname $@)
	$(CXX) -c $(CXXFLAGS) $(CFLAGS) $< -o $@

# global compilation rule (works only in submake)
$(COBJS): $(OD)/%.o : %.c
	@mkdir -p $(shell dirname $@)
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: all clean $(COMPONENTS) $(UNIT_TESTS) $(UI)

# Lib maker (actual job for submake)
%.a: $(COBJS) $(CXXOBJS)
	@echo "-------- Building static library $@ --------"
	@mkdir -p $(shell dirname $@)
	$(AR) rcs $@ $^ $(EXTERNAL_STATIC_LIBS)

# special rule to byuild unity (a C library)
# this rule may trigger previous rule, watch out for accidental erasure
$(UNITY_STATIC_LIB): $(UNITY_DIR)/src/unity.c
	@echo "-------- Building $@ --------"
	@mkdir -p $(shell dirname $@)
	@mkdir -p $(OD)
	$(CC) -c $< -I$(UNITY_DIR)/ince -o $(OD)/unity.o
	$(AR) rcs $@ $(OD)/unity.o

# binary maker
$(BINARY): $(COBJS) $(CXXOBJS)
	@echo "-------- Building binary $@ --------"
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CFLAGS) $^ $(EXTERNAL_STATIC_LIBS) $(EXTERNAL_LINK_LIBS) -o $@

# Component maker
$(COMPONENTS):
	@echo "-------- Building component $@ --------"
	$(MAKE) -C $@ OD=../$(OD)/$@ ../$(BDLIB)/$@.a

# unit testing
$(UNIT_TESTS): ut_% : $(COMPONENTS) $(UNITY_STATIC_LIB)
	@echo "-------- Building Unit testing program $@ --------"
	$(MAKE) -C ut/$* OD=../../$(OD)/$@ BINARY=../../$(BDUT)/$@ ../../$(BDUT)/$@

$(UI): % : $(COMPONENTS)
	@echo "-------- Building main binary $@ --------"
	$(MAKE) -C ui/$* OD=../../$(OD)/$@ BINARY=../../$(BD)/gc_$@ ../../$(BD)/gc_$@

clean:
	@echo "Cleaning object directory ($(OD))"
	@rm -rf $(OD)
	@echo "Cleaning bin directory ($(BD))"
	@rm -rf $(BD)

