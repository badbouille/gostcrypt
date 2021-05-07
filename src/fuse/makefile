# sub Makefile

LWEXT4_PATH=$(shell pwd)/src/fuse-lwext4/lwext4

# Parameters for LWEXT4 cmake
ifeq ($(T),debug)
LWEXT4_BUILD_TYPE := Debug
else
LWEXT4_BUILD_TYPE := Release
endif
LWEXT4_PROJECT_SETUP = "Unix Makefiles"

# platform dependent definitions
ifeq ($(P),windows)
LWEXT4_BUILD_PATH= $(LWEXT4_PATH)/build_mingw
LWEXT4_BUILD_TOOCHAIN= $(LWEXT4_PATH)/toolchain/mingw.cmake
else
LWEXT4_BUILD_PATH= $(LWEXT4_PATH)/build_generic
LWEXT4_BUILD_TOOCHAIN= $(LWEXT4_PATH)/toolchain/generic.cmake
endif

# Rule to build LWEXT4
LIBLWEXT4_A= $(LWEXT4_BUILD_PATH)/src/liblwext4.a
OTHER_DEPS+= $(LWEXT4_BUILD_PATH)/src_objects
EXTERNAL_STATIC_OBJS = $(LWEXT4_BUILD_PATH)/src_objects
# inclusions
INCLUDES:=ince inci ../common/ince ../volume/ince src/fuse-lwext4 $(LWEXT4_PATH)/include $(LWEXT4_BUILD_PATH)/include

# Windows inclusions
ifeq ($(P),windows)
INCLUDES+=src/winfsp/inc src/winfsp/inc/fuse src/winfsp/opt/fsext/inc
endif

# Objects needed
CXXFILES:= $(shell find src -maxdepth 1 -name '*.cpp')

CFILES+= src/fuse_operations.c

# C files for fuse-lwext4 (extracted from its makefile)
CFILES+= src/fuse-lwext4/op_init.c \
		 src/fuse-lwext4/op_destroy.c \
		 src/fuse-lwext4/op_open.c \
		 src/fuse-lwext4/op_create.c \
		 src/fuse-lwext4/op_release.c \
		 src/fuse-lwext4/op_read.c \
		 src/fuse-lwext4/op_write.c \
		 src/fuse-lwext4/op_truncate.c \
		 src/fuse-lwext4/op_getattr.c \
		 src/fuse-lwext4/op_opendir.c \
		 src/fuse-lwext4/op_readdir.c \
		 src/fuse-lwext4/op_releasedir.c \
		 src/fuse-lwext4/op_mkdir.c \
		 src/fuse-lwext4/op_rmdir.c \
		 src/fuse-lwext4/op_unlink.c \
		 src/fuse-lwext4/op_rename.c \
		 src/fuse-lwext4/op_chmod.c \
		 src/fuse-lwext4/op_chown.c \
		 src/fuse-lwext4/op_link.c \
		 src/fuse-lwext4/op_symlink.c \
		 src/fuse-lwext4/op_readlink.c \
		 src/fuse-lwext4/op_statvfs.c \
		 src/fuse-lwext4/op_xattr.c \
		 src/fuse-lwext4/op_utimens.c \
		 src/fuse-lwext4/acl.c \
		 src/fuse-lwext4/logging.c

$(LWEXT4_BUILD_PATH):
	mkdir -p $@
	cd $(LWEXT4_BUILD_PATH) && \
	cmake -G$(LWEXT4_PROJECT_SETUP) -DCMAKE_BUILD_TYPE=$(LWEXT4_BUILD_TYPE) \
		-DCMAKE_TOOLCHAIN_FILE=$(LWEXT4_BUILD_TOOCHAIN) \
		$(LWEXT4_PATH)

# Bonus rules to build lwext4
$(LIBLWEXT4_A): $(LWEXT4_PATH)/src/* $(LWEXT4_PATH)/include/* $(LWEXT4_BUILD_PATH)
	$(MAKE) -C $(LWEXT4_BUILD_PATH)

$(LWEXT4_BUILD_PATH)/src_objects: $(LIBLWEXT4_A)
	@mkdir -p $(LWEXT4_BUILD_PATH)/src_objects
	@rm -f $(LWEXT4_BUILD_PATH)/src_objects/*
	cd $(LWEXT4_BUILD_PATH)/src_objects && ar -x $(LIBLWEXT4_A)

# actual compilation
include rules.mk

