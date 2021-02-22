
###########################################
## Makefile for global GostCrypt Project ##
###########################################

# bin directory where the binaries and libraries are put
BD?=build

BIND?=bin
OBJD?=obj
LIBD?=lib
UTD?=$(BIND)/ut

# Available platforms
PLATFORMS:=linux windows
P?=linux

# Available targets
TARGETS:=debug release
T?=release

# Components to build
COMPONENTS:=common core crypto fuse volume unity
UNIT_TESTS:=ut_common ut_crypto ut_volume
UI:=cmdline qt

# creating paths from components
COMPONENTS_PATHS:=$(foreach i, $(PLATFORMS), $(foreach j, $(TARGETS), $(foreach k, $(COMPONENTS), $(BD)/$(i)/$(j)/$(LIBD)/$(k).a)))
UNIT_TESTS_PATHS:=$(foreach i, $(PLATFORMS), $(foreach j, $(TARGETS), $(foreach k, $(UNIT_TESTS), $(BD)/$(i)/$(j)/$(UTD)/$(k))))
UI_PATHS:=$(foreach i, $(PLATFORMS), $(foreach j, $(TARGETS), $(foreach k, $(UI), $(BD)/$(i)/$(j)/$(BIND)/gc_$(k))))

# Paths of those components
PATH_common:=src/common
PATH_core:=src/core
PATH_crypto:=src/crypto
PATH_fuse:=src/fuse
PATH_volume:=src/volume
PATH_unity:=ut/unity

PATH_ut_common:=ut/common
PATH_ut_crypto:=ut/crypto
PATH_ut_volume:=ut/volume

PATH_gc_cmdline:=src/ui/cmdline
PATH_gc_qt:=src/ui/qt

.PHONY: all clean $(COMPONENTS) $(UNIT_TESTS) $(UI) $(TARGETS) $(PLATFORMS)

# default target
all: $(UNIT_TESTS) $(UI)

# extreme target
all-all: $(UNIT_TESTS_PATHS) $(UI_PATHS)

# platform targets
linux:   $(filter $(BD)/linux/$(T)/%,   $(UNIT_TESTS_PATHS)) $(filter $(BD)/linux/$(T)/%,   $(UI_PATHS))
windows: $(filter $(BD)/windows/$(T)/%, $(UNIT_TESTS_PATHS)) $(filter $(BD)/windows/$(T)/%, $(UI_PATHS))

# target targets..
debug:   $(filter $(BD)/$(P)/debug/%,   $(UNIT_TESTS_PATHS)) $(filter $(BD)/$(P)/debug/%,   $(UI_PATHS))
release: $(filter $(BD)/$(P)/release/%, $(UNIT_TESTS_PATHS)) $(filter $(BD)/$(P)/release/%, $(UI_PATHS))

# Clean target to remove build directory
clean:
	rm -rf $(BD)

# redirection of PHONY targets to real components (files)
$(COMPONENTS): % : $(BD)/$(P)/$(T)/$(LIBD)/%.a

# redirection of PHONY targets to real components (files)
$(UNIT_TESTS): % : $(BD)/$(P)/$(T)/$(UTD)/%

# redirection of PHONY targets to real components (files)
$(UI): % : $(BD)/$(P)/$(T)/$(BIND)/gc_%

# Binary dependencies

$(filter %/ut_common, $(UNIT_TESTS_PATHS)): $(BD)/%/$(UTD)/ut_common : $(BD)/%/$(LIBD)/unity.a $(BD)/%/$(LIBD)/common.a
$(filter %/ut_crypto, $(UNIT_TESTS_PATHS)): $(BD)/%/$(UTD)/ut_crypto : $(BD)/%/$(LIBD)/unity.a $(BD)/%/$(LIBD)/common.a $(BD)/%/$(LIBD)/crypto.a
$(filter %/ut_volume, $(UNIT_TESTS_PATHS)): $(BD)/%/$(UTD)/ut_volume : $(BD)/%/$(LIBD)/unity.a $(BD)/%/$(LIBD)/common.a $(BD)/%/$(LIBD)/crypto.a $(BD)/%/$(LIBD)/volume.a

$(filter %/gc_cmdline, $(UI_PATHS)): $(BD)/%/$(BIND)/gc_cmdline: $(BD)/%/$(LIBD)/common.a $(BD)/%/$(LIBD)/crypto.a $(BD)/%/$(LIBD)/volume.a $(BD)/%/$(LIBD)/fuse.a $(BD)/%/$(LIBD)/core.a
$(filter %/gc_qt, $(UI_PATHS)): $(BD)/%/$(BIND)/gc_qt: $(BD)/%/$(LIBD)/common.a $(BD)/%/$(LIBD)/crypto.a $(BD)/%/$(LIBD)/volume.a $(BD)/%/$(LIBD)/fuse.a $(BD)/%/$(LIBD)/core.a

# Static lib maker
$(BD)/%.a: FORCE
	$(info "-------- Building static library $@ --------")
	@ # extracting platform, target and module from path
	$(eval PLATFORM=$(shell echo $* | cut -d'/' -f1))
	$(eval TARGET=$(shell echo $* | cut -d'/' -f2))
	$(eval MODULE=$(shell basename $*))
	@ # Revering module path to set a relative path to the output directory
	$(eval REVERSE_PATH=$(shell echo $(PATH_$(MODULE)) | sed 's|[^/]\+|..|g'))
	@ # Computing object directory for this module
	$(eval OD=$(REVERSE_PATH)/$(BD)/$(PLATFORM)/$(TARGET)/$(OBJD)/$(MODULE))
	@ # calling submake
	$(MAKE) -C "$(PATH_$(MODULE))" -I"$(REVERSE_PATH)/mk" P="$(PLATFORM)" T="$(TARGET)" OD="$(OD)" "$(REVERSE_PATH)/$(BD)/$*.a"

# Binary maker
$(UNIT_TESTS_PATHS) $(UI_PATHS): FORCE
	$(info "-------- Building binary $@ --------")
	@ # Getting binary relative path in the build diretory (since pattern rules can not be used here)
	$(eval BDPATH=$(shell echo "$@" | sed "s|^${BD}/||g"))
	@ # extracting platform, target and module from path
	$(eval PLATFORM=$(shell echo "$(BDPATH)" | cut -d'/' -f1))
	$(eval TARGET=$(shell echo "$(BDPATH)" | cut -d'/' -f2))
	$(eval MODULE=$(shell basename "$(BDPATH)"))
	@ # Revering module path to set a relative path to the output directory
	$(eval REVERSE_PATH=$(shell echo $(PATH_$(MODULE)) | sed 's|[^/]\+|..|g'))
	@ # Computing object directory for this module
	$(eval OD=$(REVERSE_PATH)/$(BD)/$(PLATFORM)/$(TARGET)/$(OBJD)/$(MODULE))
	@ # Computing static library directory to build binary using pre-computed libraries
	$(eval SLD=$(REVERSE_PATH)/$(BD)/$(PLATFORM)/$(TARGET)/$(LIBD))
	@ # Getting a binary name to explicitly tell the sub-make what to build
	$(eval B=$(REVERSE_PATH)/$(BD)/$(BDPATH))
	@ # calling submake (telling to build B and setting BINARY = B for a 'BINARY: ' rule in submake)
	$(MAKE) -C "$(PATH_$(MODULE))" -I"$(REVERSE_PATH)/mk" P="$(PLATFORM)" T="$(TARGET)" OD="$(OD)" STATIC_LIBSD="$(SLD)" BINARY="$(B)" "$(B)"

FORCE:
	@ # Rule to force build of submakes to check if they are up to date

