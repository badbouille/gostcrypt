
###########################################
## Makefile for global GostCrypt Project ##
###########################################

# variables to be defined by submake

# compile in debug or release mode ?
DEBUG?=yes

# flags to use for compilation
INCFLAGS?=-Iince -Iinci
CFLAGS?=

# list of files to compile for any static or dynamic target
CFILES?=

# defined by super make (location of object files)
OD?=obj

# Path for tools. Using tools from PATH by default
CC:=g++
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

# Components that can be built (folders)
COMPONENTS:=common core crypto fuse volume

# Objects needed, computed from given sources
OBJS:= $(addprefix $(OD)/, $(subst .cpp,.o,$(CFILES)))

all: clean $(COMPONENTS)

.SUFFIXES: .cpp .o .

# global compilation rule (works only in submake)
$(OD)/%.o: %.cpp
	@mkdir -p $(shell dirname $@)
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: all clean $(COMPONENTS)

# Lib maker (actual job for submake)
%.a: $(OBJS)
	@echo "-------- Building static library $@ --------"
	@mkdir -p $(shell dirname $@)
	$(AR) rcs $@ $^ $(EXTERNAL_STATIC_LIBS)

# Component maker
$(COMPONENTS):
	@echo "-------- Building component $@ --------"
	$(MAKE) -C $@ OD=../$(OD)/$@ ../$(BD)/$@.a

clean:
	@echo "Cleaning object directory ($(OD))"
	@rm -rf $(OD)
	@echo "Cleaning bin directory ($(BD))"
	@rm -rf $(BD)

