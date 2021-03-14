
###########################################
##   Makefile for sub-makes and rules    ##
###########################################

# bin directory where the binaries and libraries are put
# should be overriden by supermake
ifndef OD
$(error "No output directory for objects specified in submake")
endif

# Choosen target
ifndef T
$(error "No target defined in sub-make")
endif

# Choosen Platform
ifndef P
$(error "No platform defined in sub-make")
else
include platform/$(P).mk
endif

# finding library directory if needed
ifdef STATIC_LIBS
ifdef STATIC_LIBSD
# safest, explicit directory is given by super-make
EXTERNAL_STATIC_LIBS:=$(addprefix $(STATIC_LIBSD)/, $(STATIC_LIBS))
else
ifdef BD
# safe, if bin dir is sync with super-make
LIBD?=lib
EXTERNAL_STATIC_LIBS:=$(addprefix $(BD)/$(P)/$(T)/$(LIBD)/, $(STATIC_LIBS))
else
# unsafe, only works when OD is x/obj/module
LIBD?=lib
EXTERNAL_STATIC_LIBS:=$(addprefix $(OD)/../../$(LIBD)/, $(STATIC_LIBS))
endif
endif
endif

# Objects needed, computed from given sources
# Directories needed
ifdef CXXFILES
CXXOBJS=$(addprefix $(OD)/, $(subst .cpp,.o,$(CXXFILES)))
CXXOBJS_DIRS=$(shell dirname $(CXXOBJS) | uniq)
endif
ifdef CFILES
COBJS=$(addprefix $(OD)/, $(subst .c,.o,$(CFILES)))
COBJS_DIRS=$(shell dirname $(COBJS) | uniq)
endif
ALL_DIRS=$(shell echo $(CXXOBJS_DIRS) $(COBJS_DIRS) | tr ' ' '\n' | sort | uniq)

.SUFFIXES: .cpp .o .

# global compilation rule
$(CXXOBJS): | $(CXXOBJS_DIRS)
$(CXXOBJS): $(OD)/%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

# global compilation rule
$(COBJS): | $(COBJS_DIRS)
$(COBJS): $(OD)/%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

# Directory creation rule for objects
$(ALL_DIRS):
	@mkdir -p $@

# Lib maker
%.a: $(COBJS) $(CXXOBJS)
	@mkdir -p $(shell dirname $@)
	$(AR) rcs $@ $^ $(EXTERNAL_STATIC_LIBS)

# binary maker
$(BINARY): $(COBJS) $(CXXOBJS)
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CFLAGS) $^ $(EXTERNAL_STATIC_LIBS) $(EXTERNAL_LINK_LIBS) -o $@

