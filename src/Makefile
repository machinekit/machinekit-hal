# see http://www.cmcrossroads.com/ask-mr-make/6535-tracing-rule-execution-in-gnu-make
# to trace make execution of make in more detail:
#     make VV=1
ifeq ("$(origin VV)", "command line")
    OLD_SHELL := $(SHELL)
    SHELL = $(warning Building $@$(if $<, (from $<))$(if $?, ($? newer)))$(OLD_SHELL)
endif
# Delete the default suffix rules
.SUFFIXES:
.PHONY: default userspace modules clean modclean depclean docclean install python pythonclean cscope cscopeclean

# A "trivial build" is one which should not include dependency information
# either because it should be usable before dependency information can be
# generated or when it is invalid (clean, docclean) or when running as root
# when the user must guarantee in advance that everything is built
# (setuid, install)
ifeq ($(MAKECMDGOALS),)
TRIVIAL_BUILD=no
else
ifeq ($(filter-out clean setuid install tags swish,$(MAKECMDGOALS)),)
TRIVIAL_BUILD=yes
else
TRIVIAL_BUILD=no
endif
endif

# asciidoctor frontmatter generation for comp/instcomp
# this assumes we're running inside a git repo.
# if not, override EDIT_REMOTE EDIT_REPO EDIT_BRANCH via the environment, eg
#  EDIT_REMOTE=machinekit  make manpages  i_docpages

EDIT_REMOTE ?= origin
EDIT_REPO  ?= $(shell git ls-remote --get-url $(EDIT_REMOTE)  |sed 's/.git$$//')
EDIT_BRANCH ?= $(shell git rev-parse --abbrev-ref HEAD)
# $(info EDIT_REMOTE=$(EDIT_REMOTE) EDIT_BRANCH=$(EDIT_BRANCH)  EDIT_REPO=$(EDIT_REPO))

# Beautify output
# ---------------------------------------------------------------------------
#
# A simple variant is to prefix commands with $(Q) - that's useful
# for commands that shall be hidden in non-verbose mode.
#
#	$(Q)ln $@ :<
#
# If BUILD_VERBOSE equals 0 then the above command will be hidden.
# If BUILD_VERBOSE equals 1 then the above command is displayed.

ifeq ("$(origin V)", "command line")
  BUILD_VERBOSE = $(V)
endif
ifndef BUILD_VERBOSE
  BUILD_VERBOSE = 0
endif

ifeq ($(BUILD_VERBOSE),1)
  Q =
else
  Q = @
endif

ifeq "$(findstring s,$(MAKEFLAGS))" ""
ECHO=@echo
VECHO=echo
else
ECHO=@true
VECHO=true
endif

ifeq ($(BASEPWD),)
BASEPWD := $(shell pwd)
export BASEPWD
include Makefile.inc
ifeq ($(origin PYTHONPATH),undefined)
PYTHONPATH:=$(HAL_HOME)/lib/python
else
PYTHONPATH:=$(HAL_HOME)/lib/python:$(PYTHONPATH)
endif
export PYTHONPATH
else
include $(BASEPWD)/Makefile.inc
endif

# default target:  put before everything else
default:

#############################################################################################

OBJDIR := objects/modules
DEPDIR := depends/modules
RTLIBDIR := ../rtlib/modules

DEP = $(1) $(CPPFLAGS) -MM -MG -MT "$(2)" $(4) -o $(3).tmp && \
	mv -f "$(3)".tmp "$(3)"

cc-option = $(shell \
    if $(CC) $(CPPFLAGS) $(CFLAGS) $(1) -S -o /dev/null -xc /dev/null \
	    > /dev/null 2>&1; then \
	echo "$(1)"; \
    else \
	echo "$(2)"; \
    fi ;)

#############################################################################################


default: userspace modules

# Print 'entering' all the time
MAKEFLAGS += w

INCLUDE_PREFIX = .

# Create the variables with := so that subsequent += alterations keep it
# as a "substitute at assignment time" variable
TARGETS :=
PYTARGETS :=

# Submakefiles from each of these directories will be included if they exist
SUBDIRS := \
	\
	../man \
	machinetalk \
	machinetalk/support \
	machinetalk/messagebus \
	machinetalk/msgcomponents \
	machinetalk/lib \
	machinetalk/config-service \
	machinetalk/haltalk \
	machinetalk/mkwrapper \
	machinetalk/mklauncher \
	machinetalk/videoserver \
	\
	rtapi/rtapi_math \
	\
	inifile \
	\
	rtapi/examples/timer \
	rtapi/examples/shmem \
	rtapi/examples \
	rtapi \
	rtapi/shmdrv \
	rtapi/userpci \
	\
	hal/lib \
	hal/i_components \
	hal/components \
	hal/vtable-example \
	hal/userfunct-example \
	hal/cython \
	hal/drivers \
	hal/user_comps/devices \
	hal/user_comps/mb2hal \
	hal/user_comps \
	hal/user_icomps \
	hal/user_comps/vismach \
	hal/user_comps/vfs11_vfd \
	hal/user_comps/vfdb_vfd \
	hal/user_comps/huanyang-vfd \
	hal/user_comps/xhc-whb04b-6 \
	hal/classicladder \
	hal/utils \
	hal \
	hal/support \
	hal/drivers/hal_pru_generic \
	hal/simdrivers \
	hal/accessor \
	\
	emc/ini \
	emc \
	\
	po \

ifeq ($(BUILD_WEBTALK),yes)
SUBDIRS += machinetalk/webtalk
endif


# moved to src/rtapi/Submakefile
# ULAPISRCS := rtapi/$(RTPREFIX)_ulapi.c

# Each item in INCLUDES is transformed into a -I directive later on
# The top directory is always included
INCLUDES := .
INCLUDES += include/

USERSRCS :=
RTSRCS :=
PROGRAMS :=

# When used like $(call TOxxx, ...) these turn a list of source files
# into the corresponding list of object files, dependency files,
# or both.  When a source file has to be compiled with special flags,
# TOOBJSDEPS is used.  Confusingly, TOOBJSDEPS includes preprocessed source
# file names, but this is what allows 'make src.i' to produce proper
# preprocessed source when src.c needs a customized compile flag.
# See Submakefile.skel for an example.
TOOBJS = $(patsubst %.cc,objects/%$(2).o,$(patsubst %.c,objects/%$(2).o,$(1)))
TODEPS = $(patsubst %.cc,depends/%$(2).d,$(patsubst %.c,depends/%$(2).d,$(1)))
#-not sure, something fishy here -mah
#TODEPS = $(patsubst %.cc,objects/%.d,$(patsubst %.c,objects/%.d,$(1)))

TOOBJSDEPS = $(call TOOBJS,$(1),$(2)) $(call TODEPS, $(1),$(2))

# Same, but for RT objects in OBJDIR, compiled with -DRTAPI
TORTOBJS = $(patsubst %.cc,$(OBJDIR)/%$(2).o,$(patsubst %.c,$(OBJDIR)/%$(2).o,$(1)))

# This macro generates build rules for C components
# Example call & resulting rules:
# $$(eval $(call c_comp_build_rules,hal/components/encoder.o,hal/components/othersrc.o))
# obj-m += encoder.o
# encoder-objs := hal/components/encoder.o hal/components/othersrc.o
# $(RTLIBDIR)/encoder.so: \
#     hal/components/encoder.o hal/components/othersrc.o
define c_comp_build_rules
obj-m += $$(patsubst $$(dir $(1))%.o,%.o,$(1))
$$(patsubst $$(dir $(1))%.o,%-objs,$(1)) += $(1) $(2)
$$(patsubst $$(dir $(1))%.o,$(RTLIBDIR)/%.so,$(1)): \
    $$(addprefix $(OBJDIR)/,$(1) $(2))

endef
# Debugging
#$$(info $(call c_comp_build_rules,hal/components/encoderv2.o))

###############################################################################
# C unit test setup
#
# This macro generates rules for C unit tests; e.g.
#
# $(eval $(call setup_test, \
#     dir/test_src, RTAPI, dir/ulapi_srcs.c, dir/rtapi_srcs.c), ../lib/lib.so, \
#    -lfoo, wrap_funct)
#
# expands to:
#
# RTSRCS += dir/main_src.c
# objects/dir/test_src_RTAPI: \
#		objects/dir/ulapi_srcs.o \
#		$(OBJDIR)/dir/rtapi_srcs.o $(OBJDIR)/dir/test_src.o \
#		../lib/lib.so
#	@mkdir -p objects/dir
#	@echo Linking test objects/dir/test_src_RTAPI
#	$(Q)$(CC) -o objects/dir/test_src_RTAPI -g -DRTAPI \
#		$(OBJDIR)/dir/test_src.o \
#		objects/dir/ulapi_srcs.o $(OBJDIR)/dir/rtapi_srcs.o \
#		../lib/lib.so -lfoo -Wl,--wrap=wrap_funct
# CMOCKA_TESTS +=  dir/test_src_RTAPI
#
# ...and does the equivalent for ULAPI as well.

WL_WRAP = -Wl,--wrap=
define setup_test
$$(if $$(filter RTAPI,$(2)),RTSRCS,USERSRCS) += $(1).c
objects/$(1)_$(strip $(2)): \
		$$(call $$(if $$(filter RTAPI,$(2)),TORTOBJS,TOOBJS), $(1).c) \
		$$(call TOOBJS, $(3)) $$(call TORTOBJS, $(4)) $(5)
	@mkdir -p objects/$$(dir $(1))
	@echo Linking test objects/$(1)_$(strip $(2))
	$(Q)$(CC) -o $$@ -g $$^ -lcmocka $(6) $$(addprefix $$(WL_WRAP),$(7))
CMOCKA_TESTS += $(1)_$(strip $(2))

endef

###############################################################################

SUBMAKEFILES := $(patsubst %,%/Submakefile,$(SUBDIRS))
#$(info SUBMAKEFILES=$(SUBMAKEFILES))
-include $(wildcard $(SUBMAKEFILES))
$(info All Submakefiles included)

# This checks that all the things listed in USERSRCS are either C files
# or C++ files
ASSERT_EMPTY = $(if $(1), $(error "Should be empty but is not: $(1)"))
ifdef TARGET_PLATFORM_BEAGLEBONE
# the beaglebone port adds .p (PRU assembly) source files, see hal/components
$(call ASSERT_EMPTY,$(filter-out %.c %.cc %.p %.js  %.ph %.proto, $(USERSRCS)))
else
$(call ASSERT_EMPTY,$(filter-out %.c %.cc %.js %.proto, $(USERSRCS)))
endif
$(call ASSERT_EMPTY,$(filter-out %.c, $(RTSRCS)))

ifeq ($(BUILD_PYTHON),yes)
$(call TOOBJS,$(PYSRCS)) : EXTRAFLAGS += -fPIC $(call cc-option,-fno-strict-aliasing)
USERSRCS += $(PYSRCS)
endif

# Find the list of object files for each type of source file
CUSERSRCS := $(filter %.c,$(USERSRCS))
CXXUSERSRCS := $(filter %.cc,$(USERSRCS))
CUSEROBJS = $(call TOOBJS,$(CUSERSRCS))
CXXUSEROBJS += $(call TOOBJS,$(CXXUSERSRCS))

# Find the list of build-arch object files for C sources
CUSERSRCS_BUILD := $(filter %.c,$(USERSRCS_BUILD))
CUSEROBJS_BUILD := $(call TOOBJS,$(CUSERSRCS_BUILD),_build)

ifeq ($(TRIVIAL_BUILD),no)

ifeq ($(USE_PROTOBUF),yes)
# force create of %.proto-dependent files and their deps
Makefile: $(GENERATED) $(PROTO_DEPS)
-include $(PROTO_DEPS)
endif

ifdef TARGET_PLATFORM_BEAGLEBONE
ifneq ($(PRU_DEPS),)
Makefile: $(PRU_DEPS)
READ_PRU_DEPS = $(wildcard $(PRU_DEPS))
$(shell echo 1>&2 Reading $(words $(READ_PRU_DEPS))/$(words $(PRU_DEPS)) PRU dependency files)
-include $(wildcard $(READ_PRU_DEPS))
endif
endif

# Find the dependency filenames, then include them all
DEPS := $(sort $(patsubst %.o,%.d,$(CUSEROBJS) $(CXXUSEROBJS) \
	$(CUSEROBJS_BUILD)))
READ_DEPS = $(wildcard $(DEPS))
$(shell echo 1>&2 Reading $(words $(READ_DEPS))/$(words $(DEPS)) dependency files)
-include $(READ_DEPS)
UNREAD_DEPS = $(filter-out $(READ_DEPS), $(DEPS))
LEGACY_DEPS = $(patsubst objects/%, depends/%, $(UNREAD_DEPS))
READ_LEGACY_DEPS = $(wildcard $(LEGACY_DEPS))
ifneq ($(READ_LEGACY_DEPS),)
$(shell echo 1>&2 Reading $(words $(READ_LEGACY_DEPS)) old-style dependency files)
-include $(READ_LEGACY_DEPS)
endif
$(shell echo 1>&2 Done reading dependencies)
endif

# Each directory in $(INCLUDES) is passed as a -I directory when compiling.
INCLUDE := $(patsubst %,-I%, $(INCLUDES))
ifeq ($(BUILD_PYTHON),yes)
INCLUDE += $(PYTHON_CPPFLAGS)
endif

# trap build errors by making it possible to compare configure SHA vs build SHA
# see configure.ac GIT_CONFIG_SHA for the configure-time string
GIT_BUILD_SHA := $(shell ../scripts/get-git-sha)

# Compilation options.	Perhaps some of these should come from Makefile.inc? (CXXFLAGS now does)
INTEGER_OVERFLOW_FLAGS := $(call cc-option,-fwrapv) $(call cc-option, -fno-strict-overflow)
OPT :=  $(INTEGER_OVERFLOW_FLAGS)
DEBUG := -g -Wall -funwind-tables
PROFILE_CFLAGS :=
PROFILE_LDFLAGS :=

CFLAGS := $(INCLUDE) $(OPT) $(DEBUG) $(DPKG_CFLAGS) $(PROFILE_CFLAGS) \
	-DULAPI $(call cc-option,-std=gnu99 -fgnu89-inline)

# make ck_pr.h happy:
CFLAGS += $(CK_CFLAGS)

# if not initialised use innocuous switch to prevent errors
ifeq ($(ARCH_CFLAGS),@ARCH_CFLAGS@)
ARCH_CFLAGS := -Wunused
endif

BASE_CXXFLAGS:= $(INCLUDE) $(CXXFLAGS) \
		$(CK_CFLAGS) \
		$(DEBUG) $(DPKG_CFLAGS) $(OPT) \
		$(PROFILE_CFLAGS) \
		$(ARCH_CFLAGS)

CXXFLAGS :=   $(BASE_CXXFLAGS) -DULAPI
RT_CXXFLAGS :=   $(BASE_CXXFLAGS)

ifeq ($(RUN_IN_PLACE),yes)
LDFLAGS += -L$(LIB_DIR) -Wl,-rpath,$(LIB_DIR)
else
LDFLAGS += -Wl,-rpath-link,../lib
endif
LDFLAGS += -Wl,--no-as-needed

# Rules to make .o (object) files
$(sort $(CUSEROBJS)) : objects/%.o: %.c
	$(ECHO) Compiling $<
	@mkdir -p $(dir $@)
	@rm -f $@
	$(Q)$(CC) -c $(CPPFLAGS) $(CFLAGS) $(EXTRAFLAGS) \
		-MP -MD -MF "${@:.o=.d}" -MT "$@" \
		$< -o $@

# Rules to make .o (object) files, build arch
$(sort $(CUSEROBJS_BUILD)) : objects/%_build.o: %.c
	$(ECHO) Compiling $<
	@mkdir -p $(dir $@)
	@rm -f $@
	$(Q)$(CC_FOR_BUILD) -c $(CFLAGS) $(EXTRAFLAGS) \
		-MP -MD -MF "${@:.o=.d}" -MT "$@" \
		$< -o $@

$(sort $(CXXUSEROBJS)) : objects/%.o: %.cc
	$(ECHO) Compiling ++ $<
	@mkdir -p $(dir $@)
	@rm -f $@
	$(Q)$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(EXTRAFLAGS) \
		-MP -MD -MF "${@:.o=.d}" -MT "$@" \
		$< -o $@

ifeq ($(TRIVIAL_BUILD),no)
configure: configure.ac
	./autogen.sh

config.h.in: configure.ac
	./autogen.sh

config.status: configure
	if [ -f config.status ]; then ./config.status --recheck; else \
	    echo 1>&2 "*** machinekit is not configured.	Run './configure' with appropriate flags."; \
	    exit 1; \
	fi
endif

Makefile: config.h
config.h: config.h.in config.status
	@./config.status -q --header=$@

INFILES = \
	../scripts/realtime \
	../scripts/gen-rtapi.ini.sh \
	Makefile.inc \
	Makefile.modinc \
	../scripts/halrun \
	../scripts/rip-environment

$(INFILES): %: %.in config.status
	@./config.status --file=$@

default: $(INFILES)

# For each file to be copied to ../include, its location in the source tree
# is listed here.  Note that due to $(INCLUDE), defined above, the include
# files in the source tree are the ones used when building machinekit.  The copy
# in ../include is used when building external components of machinekit.
HEADERS := \
    hal/lib/hal_accessor.h \
    hal/lib/hal_accessor_macros.h \
    hal/lib/config_module.h \
    hal/lib/hal_group.h \
    hal/lib/hal.h \
    hal/lib/hal_iring.h \
    hal/lib/hal_internal.h \
    hal/lib/hal_iter.h \
    hal/lib/hal_list.h \
    hal/lib/hal_logging.h \
    hal/lib/hal_object.h \
    hal/lib/hal_object_selectors.h \
    hal/lib/hal_priv.h \
    hal/lib/hal_rcomp.h \
    hal/lib/hal_ring.h \
    hal/lib/hal_types.h \
    hal/lib/vtable.h \
    hal/drivers/hal_spi.h \
    inifile/mk-inifile.h \
    inifile/mk-inifile.hh \
    machinetalk/build/machinetalk/protobuf/message.pb.h \
    machinetalk/build/machinetalk/protobuf/types.pb.h \
    machinetalk/build/machinetalk/protobuf/canon.npb.h \
    machinetalk/build/machinetalk/protobuf/config.pb.h \
    machinetalk/build/machinetalk/protobuf/firmware.npb.h \
    machinetalk/build/machinetalk/protobuf/jplan.pb.h \
    machinetalk/build/machinetalk/protobuf/sample.pb.h \
    machinetalk/build/machinetalk/protobuf/message.npb.h \
    machinetalk/build/machinetalk/protobuf/motcmds.pb.h \
    machinetalk/build/machinetalk/protobuf/object.npb.h \
    machinetalk/build/machinetalk/protobuf/preview.pb.h \
    machinetalk/build/machinetalk/protobuf/rtapicommand.npb.h \
    machinetalk/build/machinetalk/protobuf/rtapi_message.pb.h \
    machinetalk/build/machinetalk/protobuf/task.npb.h \
    machinetalk/build/machinetalk/protobuf/test.pb.h \
    machinetalk/build/machinetalk/protobuf/value.npb.h \
    machinetalk/build/machinetalk/protobuf/canon.pb.h \
    machinetalk/build/machinetalk/protobuf/emcclass.npb.h \
    machinetalk/build/machinetalk/protobuf/firmware.pb.h \
    machinetalk/build/machinetalk/protobuf/log.npb.h \
    machinetalk/build/machinetalk/protobuf/nanopb.npb.h \
    machinetalk/build/machinetalk/protobuf/object.pb.h \
    machinetalk/build/machinetalk/protobuf/ros.npb.h \
    machinetalk/build/machinetalk/protobuf/rtapicommand.pb.h \
    machinetalk/build/machinetalk/protobuf/status.npb.h \
    machinetalk/build/machinetalk/protobuf/task.pb.h \
    machinetalk/build/machinetalk/protobuf/types.npb.h \
    machinetalk/build/machinetalk/protobuf/value.pb.h \
    machinetalk/build/machinetalk/protobuf/config.npb.h \
    machinetalk/build/machinetalk/protobuf/emcclass.pb.h \
    machinetalk/build/machinetalk/protobuf/jplan.npb.h \
    machinetalk/build/machinetalk/protobuf/sample.npb.h \
    machinetalk/build/machinetalk/protobuf/log.pb.h \
    machinetalk/build/machinetalk/protobuf/motcmds.npb.h \
    machinetalk/build/machinetalk/protobuf/nanopb.pb.h \
    machinetalk/build/machinetalk/protobuf/preview.npb.h \
    machinetalk/build/machinetalk/protobuf/ros.pb.h \
    machinetalk/build/machinetalk/protobuf/rtapi_message.npb.h \
    machinetalk/build/machinetalk/protobuf/status.pb.h \
    machinetalk/build/machinetalk/protobuf/test.npb.h \
    machinetalk/nanopb/pb.h \
    machinetalk/nanopb/pb_common.h \
    machinetalk/nanopb/pb_encode.h \
    machinetalk/nanopb/pb_decode.h \
    rtapi/multiframe_flag.h \
    rtapi/rtapi.h \
    rtapi/rtapi_app.h \
    rtapi/rtapi_atomics.h \
    rtapi/rtapi_mutex.h \
    rtapi/rtapi_bitops.h \
    rtapi/rtapi_byteorder.h \
    rtapi/rtapi_export.h \
    rtapi/rtapi_compat.h \
    rtapi/rtapi_hexdump.h \
    rtapi/rtapi_int.h \
    rtapi/rtapi_limits.h \
    rtapi/rtapi_math.h \
    rtapi/rtapi_math64.h \
    rtapi/rtapi_stdint.h \
    rtapi/rtapi_bool.h \
    rtapi/rtapi_common.h \
    rtapi/rtapi_exception.h \
    rtapi/rtapi_global.h \
    rtapi/rtapi_shmkeys.h \
    rtapi/rtapi_errno.h \
    rtapi/rtapi_string.h \
    rtapi/rtapi_pci.h \
    rtapi/rtapi_heap.h \
    rtapi/rtapi_heap_private.h \
    rtapi/ring.h \
    rtapi/triple-buffer.h \
    rtapi/multiframe.h \
    rtapi/rtapi_mbarrier.h \
    rtapi/shmdrv/shmdrv.h \
    rtapi/flavor/rtapi_flavor.h \
    rtapi/flavor/xenomai2.h \
    rtapi/flavor/rt-preempt.h \
    rtapi/flavor/ulapi.h

ifeq ($(HAVE_SYS_IO),yes)
    HEADERS += \
        rtapi/rtapi_io.h
endif
ifeq ($(TARGET_PLATFORM_PC),yes)
    HEADERS += \
        hal/lib/hal_parport.h
endif

## the "headers" target installs all the header files in ../include
.PHONY: headers
HEADERS := $(patsubst %,../include/%,$(foreach h,$(HEADERS),$(notdir $h)))
headers: $(HEADERS)

# install header files as part of the build
TARGETS += headers

# Add headers to this list that need to go into subdirectories; their
# respective Submakefiles will need to provide rules to copy them into
# ../install.
headers: $(SUBDIRECTORY_HEADERS)

# And make userspace depend on $(TARGETS)
userspace: $(TARGETS)

ifeq ($(BUILD_PYTHON),yes)
pythonclean:
	rm -f $(PYTARGETS)
	find ../lib/python -name '*.so' -exec rm {} +
python: $(PYTARGETS)
userspace: python
clean: pythonclean cscopeclean
endif

# These rules clean things up.	'modclean' cleans files generated by 'modules'
# (except that it doesn't remove the modules that were copied to rtlib)
# 'clean' cleans everything but dependency files, and 'depclean' cleans them
# too.
modclean:
	-rm -f $(RTLIBDIR)/*.so

depclean:
	-rm -rf depends

docclean:
	-rm -f ../man/man9/*.9icomp
	-rm -f ../man/doc/man9/*.asciidoc

clean: depclean modclean docclean
	find . -name '*.o' |xargs rm -f
	-rm -rf objects
	-rm -f $(TARGETS)
	-rm -rf ../rtlib ../libexec
	rm -f ../etc/machinekit/*.ini
	rm -f ../etc/machinekit/*.conf
	-rm -f $(COPY_CONFIGS)
	-rm -f $(RTLIBDIR)/*.so
	-rm -f hal/components/conv_*.comp
	-rm -f hal/i_components/conv_*.icomp
	-rm -f hal/components/*_bin.h   # generated by pasm for bb platform
	-rm -rf ../libexec
	-rm -f ../include/*.hh ../include/*.h

# So that nothing is built as root, this rule does not depend on the touched
# files (Note that files in depends/ might be rebuilt, and there's little that
# can be done about it)
fix_perms = test ! -f $(1) || (chown root $(1) && chmod 4750 $(1))

ifeq ($(BUILD_DRIVERS),yes)
setuid:
ifeq ($(HAVE_SYS_IO),yes)
	$(call fix_perms,../libexec/pci_read)
	$(call fix_perms,../libexec/pci_write)
endif
	$(call fix_perms,../libexec/rtapi_app)
else
setuid:
	@echo "'make setuid' is not needed if hardware drivers are not used"
endif

# These rules allows a header file from this directory to be installed into
# ../include.  A pair of rules like these will exist in the Submakefile
# of each file that contains headers.
../include/%.h: %.h
	$(ECHO) Copying header file $@
	@mkdir -p $(dir $@)
	$(Q)-cp $^ $@
../include/%.hh: %.hh
	$(ECHO) Copying header file $@
	@mkdir -p $(dir $@)
	$(Q)-cp $^ $@


DIR=install -d -m 0755 -o root
FILE=install -m 0644 -o root
TREE=cp -dR
CONFIGFILE=install -m 0644
EXE=install -m 0755 -o root
SETUID=install -m 4755 -o root
GLOB=$(wildcard $(1))

ifeq ($(RUN_IN_PLACE),yes)
define ERROR_MESSAGE
You configured run-in-place, but are trying to install.
For an installable version, run configure without --enable-run-in-place
and rebuild
endef
install:
	$(error $(ERROR_MESSAGE))

else  ## ifeq($(RUN_IN_PLACE),yes)

DOCS_HELP=$(call GLOB,../help/*)
NC_FILES=$(filter-out %/butterfly.ngc,$(call GLOB,../nc_files/*))

install: install-kernel-dep install-kernel-indep
	$(ECHO) "Installed in $(DESTDIR) with prefix $(prefix)"

install-dirs:
	$(DIR)  $(DESTDIR)$(HAL_RTLIB_DIR) \
		$(DESTDIR)$(sysconfdir)/machinekit $(DESTDIR)$(bindir) \
		$(DESTDIR)$(includedir)/machinekit \
		$(DESTDIR)$(docdir) $(DESTDIR)$(ncfilesdir) \
		$(DESTDIR)$(HAL_LIBEXEC_DIR) \
		$(DESTDIR)$(mandir)/man1 \
		$(DESTDIR)$(mandir)/man3 \
		$(DESTDIR)$(mandir)/man9 \
		$(DESTDIR)$(sysconfdir)/rsyslog.d \
		$(DESTDIR)$(sysconfdir)/security/limits.d \
		$(DESTDIR)$(sysconfdir)/udev/rules.d \
		$(DESTDIR)$(sampleconfsdir) \
		$(DESTDIR)$(datadir)/machinekit \
		$(DESTDIR)$(datadir)/pkgconfig
	$(DIR)	$(DESTDIR)$(includedir)/machinekit/userpci

install-kernel-indep: install-dirs
	$(EXE) ../scripts/realtime $(DESTDIR)$(bindir)
	$(EXE) ../scripts/halrun $(DESTDIR)$(bindir)
	$(EXE) ../scripts/runtests $(DESTDIR)$(bindir)
	$(EXE) $(filter ../bin/%,$(TARGETS)) $(DESTDIR)$(bindir)
	$(FILE) $(filter ../lib/%.a ../lib/%.so.0,$(TARGETS)) $(DESTDIR)$(libdir)
	cp --no-dereference $(filter ../lib/%.so, $(TARGETS)) $(DESTDIR)$(libdir)
#	# don't run ldconfig under fakeroot (silence dpkg-build warning)
	-test -n "$$FAKED_MODE" || ldconfig $(DESTDIR)$(libdir)
	$(FILE) $(HEADERS) $(DESTDIR)$(includedir)/machinekit/
	$(FILE) $(USERPCI_HEADERS) $(DESTDIR)$(includedir)/machinekit/userpci
	$(FILE) Makefile.modinc $(DESTDIR)$(datadir)/machinekit
	$(FILE) Makefile.inc $(DESTDIR)$(datadir)/machinekit
	$(FILE) ../share/pkgconfig/machinekit-hal.pc \
	    $(DESTDIR)$(datadir)/pkgconfig

	$(FILE) rtapi/rsyslogd-hal.conf $(DESTDIR)$(sysconfdir)/rsyslog.d/hal.conf
	$(FILE) rtapi/shmdrv/limits.d-machinekit.conf \
		$(DESTDIR)$(sysconfdir)/security/limits.d/machinekit.conf
	$(FILE) ../etc/machinekit/machinekit.ini $(DESTDIR)$(sysconfdir)/machinekit
	$(EXE) ../bin/mank $(DESTDIR)$(bindir)

ifeq ($(BUILD_PYTHON),yes)
install-kernel-indep: install-python
install-python: install-dirs
	$(DIR) $(DESTDIR)$(SITEPY)/machinekit
	$(DIR) $(DESTDIR)$(SITEPY)/machinetalk
	$(DIR) $(DESTDIR)$(SITEPY)/machinetalk/protobuf
	$(DIR) $(DESTDIR)$(SITEPY)/fdm
	$(DIR) $(DESTDIR)$(SITEPY)/drivers
	$(EXE) ../bin/hal_input $(DESTDIR)$(bindir)
	$(EXE) ../bin/hal_gpio_mcp23017 $(DESTDIR)$(bindir)
	$(EXE) ../bin/hal_pwm_pca9685 $(DESTDIR)$(bindir)
	$(EXE) ../bin/hal_storage $(DESTDIR)$(bindir)
	$(EXE) ../bin/hal_temp_ads7828 $(DESTDIR)$(bindir)
	$(EXE) ../bin/hal_temp_bbb $(DESTDIR)$(bindir)
	$(EXE) ../bin/hal_temp_atlas $(DESTDIR)$(bindir)
	$(FILE) ../lib/python/*.py ../lib/python/*.so $(DESTDIR)$(SITEPY)
	$(FILE) ../lib/python/machinekit/*.py $(DESTDIR)$(SITEPY)/machinekit/
	$(FILE) ../lib/python/machinekit/*.so $(DESTDIR)$(SITEPY)/machinekit/
	$(FILE) ../lib/python/machinetalk/*.py $(DESTDIR)$(SITEPY)/machinetalk/
	$(FILE) ../lib/python/machinetalk/protobuf/*.py $(DESTDIR)$(SITEPY)/machinetalk/protobuf/
	$(FILE) ../lib/python/drivers/*.py $(DESTDIR)$(SITEPY)/drivers
	$(EXE) ../bin/mkwrapper $(DESTDIR)$(bindir)
	$(EXE) ../bin/mklauncher $(DESTDIR)$(bindir)
	$(EXE) ../bin/configserver $(DESTDIR)$(bindir)
	$(EXE) ../bin/videoserver $(DESTDIR)$(bindir)
endif

install-kernel-dep: install-dirs
	$(SETUID) ../libexec/rtapi_app $(DESTDIR)$(HAL_LIBEXEC_DIR)
ifeq ($(BUILD_DRIVERS),yes)
ifeq ($(HAVE_SYS_IO),yes)
	$(SETUID) ../libexec/pci_write $(DESTDIR)$(HAL_LIBEXEC_DIR)
	$(SETUID) ../libexec/pci_read $(DESTDIR)$(HAL_LIBEXEC_DIR)
endif
endif
	$(EXE) ../libexec/inivar $(DESTDIR)$(HAL_LIBEXEC_DIR)
	$(EXE) ../libexec/rtapi_msgd $(DESTDIR)$(HAL_LIBEXEC_DIR)
	$(FILE) ../etc/machinekit/rtapi.ini $(DESTDIR)$(sysconfdir)/machinekit

	# RTAPI modules:  install userland flavor .so modules into
	# e.g. /usr/lib/machinekit/modules

	echo Installing modules
	$(DIR) $(DESTDIR)$(HAL_RTLIB_BASE_DIR)/modules
	$(FILE) ../rtlib/modules/* \
		    $(DESTDIR)$(HAL_RTLIB_BASE_DIR)/modules

endif # ! RUN_IN_PLACE

CONF=../configs
COMMON=$(CONF)/common
CONFILES=$(addsuffix /$(1), $(filter-out $(COMMON) $(CONF),\
                                         ${shell find ${CONF} -type d -print}))

#################################################################################################################

EXTRA_CFLAGS += \
        $(ARCH_CFLAGS) \
	-D__MODULE__ \
	-I$(INCLUDE_PREFIX) \
	-I$(INCLUDE_PREFIX)/inifile \
	-I$(INCLUDE_PREFIX)/rtapi \
	-I$(INCLUDE_PREFIX)/rtapi/flavor \
	-I$(INCLUDE_PREFIX)/hal/lib \
	-I$(INCLUDE_PREFIX)/machinetalk/nanopb \
	-I$(INCLUDE_PREFIX)/machinetalk/build \
	-DSEQUENTIAL_SUPPORT -DHAL_SUPPORT -DDYNAMIC_PLCSIZE -DRT_SUPPORT \
	-DOLD_TIMERS_MONOS_SUPPORT -DMODBUS_IO_MASTER \
	$(call cc-option,-mieee-fp) \
	$(KERNEL_MATH_CFLAGS)

ifdef TARGET_PLATFORM_BEAGLEBONE
EXTRA_CFLAGS += -I$(INCLUDE_PREFIX)/hal/support/pru
endif

################################################################################
# Misc C components in directories with no Submakefile
$(eval $(call c_comp_build_rules,hal/jplanner/jplan.o))
$(eval $(call c_comp_build_rules,hal/interpolator/interpolate.o, \
     machinetalk/build/machinetalk/protobuf/ros.npb.o))
$(eval $(call c_comp_build_rules,hal/icomp-example/icomp.o))
# clashes with component in i_components
$(eval $(call c_comp_build_rules,hal/icomp-example/lutn-demo.o))

TOCOMPRTOBJS = $(foreach file,$($(patsubst %.o,%,$(1))-objs), $(OBJDIR)/$(file))

#######################################################################################################

# Module building
EXTRA_CFLAGS += -fPIC

# Non-module RT objects
RTOBJS = $(call TORTOBJS,$(RTSRCS))
# Module RT objects
RTOBJS += $(sort $(foreach mod,$(obj-m),$(call TOCOMPRTOBJS,$(mod))))

RTDEPS := $(sort $(patsubst $(OBJDIR)/%.o,$(DEPDIR)/%.d, $(RTOBJS)))

# this arcane step warrants some explanation.

# this is about linking userland RT modules to be loaded by rtapi_app
# Those modules are supposed to behave the same as kernel RT modules as far as
# symbol visibility goes. Other than in plain C, in-kernel symbol
# export is explicitly controlled by the EXPORT_SYMBOL(sym) macro.
# only variables and functions tagged such will be visible to the rest of the kernel
# and other modules.
# See http://stackoverflow.com/questions/9836467/whats-meaning-of-export-symbol-in-linux-kernel-code
# for an explanation and use.
#
# userland RT modules need to follow the same visibility rules as kernel modules
# to prevent accidential spillover of symbols between components (which does not happen
# in kernel space).
#
# The kernel behavior of EXPORT_SYMBOL is emulated as follows:
# rtapi_export.h defines
# #define EXPORT_SYMBOL(x) __attribute__((section(".rtapi_export"))) \
#    char rtapi_exported_##x[] = #x;
#
# This means any symbol 'foo' exported as EXPORT_SYMBOL(foo) will:
# - have a variable defined which looks like so:
#   char rtapi_exported_foo = "foo";
# - have put this variable put in the .rtapi_export section of the object file.
#   (see http://wiki.osdev.org/ELF_Tutorial#The_String_Table)
# - this string table is different from the default string table named .srtab.
#
# Hence, the .rtapi_export section of a RT module is a pure string table; no other
# variables live there. The strings in this table is the set of symbols exported by
# this module.
#
# Now, during linking, all symbols which are NOT in this set need to be removed.
#
# to do so, first the .rtapi_export section (the string table) is extracted and stored
# in a file - one can reproduce with:
#
# objcopy -j .rtapi_export -O binary objects/posix/hal_lib.tmp /dev/stdout | strings
# rtapi_info_author
# rtapi_info_description
# rtapi_info_license
# rtapi_app_main
# rtapi_app_exit
# hal_init
# hal_xinit
# hal_xinitf
# hal_ready
# ....
#
# In the second objcopy step, all symbols not in this set are removed. This is done
# by determining the list of symbols to be kept, and generating a list of lines like
# -G <symbol>
# which is the objcopy option for removing all other symbols not in this list:
# -G symbolname
# --keep-global-symbol=symbolname
#   Keep only symbol symbolname global.  Make all other symbols local to the file, so that they are not
#   visible externally.  This option may be given more than once.
# This objcopy step is in-place, i.e. overwriting the .tmp file.
#
# The final linking step uses the -Bsymbolic flag - what this does is:
#
# This element's presence in a shared object library alters the dynamic linker's symbol resolution
# algorithm for references within the library. Instead of starting a symbol search with the
# executable file, the dynamic linker starts from the shared object itself. If the shared object
# fails to supply the referenced symbol, the dynamic linker then searches the executable
# file and other shared objects as usual.
# (from https://www.technovelty.org/c/what-exactly-does-bsymblic-do.html)
#
# this is important for symbol resolution when the .so is loaded within rtapi_app:
# it means that symbols referenced are resolved within the object first, instead of looking
# at any symbols exported (maybe by accident) by rtap_app proper.
#

modules:
ifeq ($(RUN_IN_PLACE)+$(BUILD_DRIVERS),yes+yes)
	@test -f ../libexec/rtapi_app -a \
	    \( 0`stat -c %u ../libexec/rtapi_app 2>/dev/null` \
			-ne 0 -o ! -u ../libexec/rtapi_app \) \
		&& need_setuid=1; \
	test "$$need_setuid" = 1 && \
	    $(VECHO) -n "You now need to run 'sudo make setuid' " && \
	    $(VECHO) "in order to run in place." || true
endif

modules: $(patsubst %.o,$(RTLIBDIR)/%.so,$(obj-m))
$(RTLIBDIR)/%.so:
	$(ECHO) Linking realtime $(notdir $@)
	@mkdir -p $(dir $@)
	@# link all objects files into a single .so
	$(Q)$(LD) -d -r -o $(OBJDIR)/$*.tmp $^
	@# use the .rtapi_export string table to generate a ld version script
	@# explicitly defining the EXPORT_SYMBOL syms as global, and everything else as local:
	$(Q)$(OBJCOPY) -j .rtapi_export -O binary $(OBJDIR)/$*.tmp $(OBJDIR)/$*.exported
	$(Q)(echo '{ global : ';  tr -s '\0' <$(OBJDIR)/$*.exported | xargs -r0 printf '%s;\n' | grep .; echo 'local : * ; };') > $(OBJDIR)/$*.ver
	@# link the final object using this version script:
	$(Q)$(CC) -shared -Bsymbolic $(LDFLAGS) -Wl,--version-script,$(OBJDIR)/$*.ver -o $@ $^ $(EXTRA_LDFLAGS)

$(sort $(RTDEPS)): $(DEPDIR)/%.d: %.c
	@mkdir -p $(dir $@)
	$(ECHO) Depending realtime $<
	$(Q)$(call DEP,$(CC),$@ \
		$(patsubst depends/%.d,objects/%.o,$@),$@,$(OPT) \
		$(DEBUG) $(CPPFLAGS) $(EXTRA_CFLAGS) $<)

# Rules to make .o (object) files
$(sort $(RTOBJS)) : $(OBJDIR)/%.o : %.c $(DEPDIR)/%.d
	$(ECHO) Compiling realtime $<
	@rm -f $@
	@mkdir -p $(dir $@)
	$(Q)$(CC) -c $(OPT) $(DPKG_CFLAGS) $(DEBUG) $(CPPFLAGS) \
	    $(EXTRA_CFLAGS) $< -o $@

# Rules to make .o (object) files from .cc
$(sort $(CXXRTOBJS)) : $(OBJDIR)/%.o : %.cc $(DEPDIR)/%.d
	$(ECHO) Compiling realtime++ $<
	@rm -f $@
	@mkdir -p $(dir $@)
	$(Q)$(CXX) -c $(OPT) $(DEBUG) $(RT_CXXFLAGS) $(CPPFLAGS) \
	    $(EXTRA_CFLAGS) $< -o $@

###############################################################################################


# Phony so that it is always rebuilt when requested, not because it
# shouldn't exist as a file
.PHONY: tags

# dont create tags from these directories
NOTAGSIN := \
	depends \
	objects \
	machinetalk/build \
	machinetalk/nanopb

CTAGOPTS := $(addprefix --exclude=,$(NOTAGSIN))
ETAGOPTS := $(CTAGOPTS)
FINDOPTS :=  -not \( $(patsubst %,-path ./% -prune -o,$(NOTAGSIN)) -type d \)

tags:
	ctags-exuberant --extra=+fq \
		$(CTAGOPTS) \
		'--langmap=make:+(Submakefile),make:+(Makefile.inc),c:+.comp' \
		-I EXPORT_SYMBOL+,RTAPI_MP_INT+,RTAPI_MP_LONG+,RTAPI_MP_STRING+ \
		-I RTAPI_MP_ARRAY_INT+,RTAPI_MP_ARRAY_LONG+,RTAPI_MP_ARRAY_STRING+ \
		-I MODULE_AUTHOR+,MODULE_DESCRIPTION+,MODULE_LICENSE+ \
		-R . ../scripts
	rm -f TAGS
	find . -type f -name '*.[ch]' $(FINDOPTS) | xargs  etags -l c --append
	find . -type f -name '*.cc'  $(FINDOPTS) | xargs  etags -l c++ --append
	find . -type f -name '*.hh'  $(FINDOPTS) | xargs  etags -l c++ --append
	find . -type f -name '*.proto'  $(FINDOPTS) | xargs  etags -l c++ --append

# etags from exuberant-ctags package
etags:
	etags --extra=+fq \
		$(ETAGOPTS) \
		'--langmap=make:+(Submakefile),make:+(Makefile.inc),c:+.comp' \
		-I EXPORT_SYMBOL+,RTAPI_MP_INT+,RTAPI_MP_LONG+,RTAPI_MP_STRING+ \
		-I RTAPI_MP_ARRAY_INT+,RTAPI_MP_ARRAY_LONG+,RTAPI_MP_ARRAY_STRING+ \
		-I MODULE_AUTHOR+,MODULE_DESCRIPTION+,MODULE_LICENSE+ \
		-R . ../scripts
	find . -type f -name '*.[ch]' $(FINDOPTS) |xargs etags --language-force=C --append
	find . -type f -name '*.cc'  $(FINDOPTS) |xargs etags --language-force=C++ --append
	find . -type f -name '*.hh'  $(FINDOPTS) |xargs etags --language-force=C++ --append
	find . -type f -name '*.proto'  $(FINDOPTS) |xargs etags --language-force=C++ --append

# emacs etags
eetags:
	@rm -f TAGS
	@find .  $(FINDOPTS) -type f -name '*.[ch]'  |xargs etags --language=c   --append
	@find .  $(FINDOPTS) -type f -name '*.cc'    |xargs etags --language=c++ --append
	@find .  $(FINDOPTS) -type f -name '*.hh'    |xargs etags --language=c++ --append
	@find .  $(FINDOPTS) -type f -name '*.proto' |xargs etags --language=c++ --append

.PHONY: swish
swish:
	swish-e -c .swish_config -v 0 -i $(BASEPWD) \
		$(dir $(BASEPWD))lib/python \
		$(dir $(BASEPWD))scripts \
		$(dir $(BASEPWD))configs

# When you depend on objects/var-ZZZ you are depending on the contents of the
# variable ZZZ, which is assumed to depend on a Makefile, a Submakefile, or
# Makefile.inc
objects/var-%: Makefile $(wildcard $(SUBMAKEFILES)) Makefile.inc
	@mkdir -p $(dir $@)
	@echo $($*) > $@.tmp
	@sh move-if-change $@.tmp $@

# Link shared libs
../lib/%.so: ../lib/%.so.0
	$(ECHO) Symlinking $(notdir $<) to $(notdir $@)
	$(Q)ln -sf $(notdir $<) $@
# - Be sure the setuid message comes last
ALL_SHLIBS=$(filter ../%.so.0,$(TARGETS)) $(filter ../%.so,$(TARGETS))
modules: $(ALL_SHLIBS)

cscope:
	cscope -Rb

cscope/man/:
	bash -c 'for f in `find ./ -name "cscope.*out"`;do rm $$f;done'

###############################################################################
# C unit tests
#
# Run with `make cmocka_tests`

cmocka_tests:  $(addprefix objects/, $(CMOCKA_TESTS))
	@for test in $^; do \
	    echo "\nRunning test $$test"; \
	    $$test; \
	done
