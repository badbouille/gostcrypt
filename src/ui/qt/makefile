# sub Makefile (qt special)

# bin directory where the binaries and libraries are put
# should be overriden by supermake
ifndef OD
$(error "No output directory for objects specified in submake")
endif

# Choosen Platform
ifndef P
$(error "No platform defined in sub-make")
endif

# Checking if needed var is defined
ifndef STATIC_LIBSD
$(error "Library directory is not defined"
endif

# Additional flags useful to build final binary
ifeq ($(T),release)
CFLAGS += -flto
endif

# binary maker
.PHONY: $(BINARY)
$(BINARY):
	@echo "-------- Building Qt binary $@ --------"
	# Getting binary name
	$(eval BNAME:=$(shell basename $@))
	# Getting binary path
	$(eval DDIR:=$(shell dirname $@))
	# Calling qmake
	qmake -o Makefile_qt STATIC_LIBSD=$(STATIC_LIBSD) BNAME=$(BNAME) DDIR=$(DDIR) OBJD=$(OD) QMAKE_CXXFLAGS=$(CFLAGS)
	$(MAKE) -f Makefile_qt

# modules that have to be installed
# qt5-default qtdeclarative5-dev
# qml-module-qtquick2 qml-module-qtquick-controls qml-module-qtquick-layouts qml-module-qtquick-dialogs
# qml-module-qt-labs-folderlistmodel qml-module-qt-labs-settings
