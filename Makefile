###########################
# AUTHOR: haeram27@gmail.com
# DATE: 20200627
###########################

###########################
# PATHS
###########################
PROJECT_ROOT := $(abspath $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST)))))
EXECUTABLE := a.out
BUILDDIR := build

OBJDIR = $(BUILDDIR)/obj
APPDIR = $(BUILDDIR)/bin

SRCROOTDIR := src
SRCDIRS = $(shell find $(SRCROOTDIR) -type d)
SRCINC = $(addprefix -I,$(SRCROOTDIR))

HDREXT := h hh hpp hxx h++
CPPEXT := cc cpp
CXXSRCS = $(foreach dir,$(SRCDIRS),\
          $(foreach ext,$(CPPEXT),$(wildcard $(dir)/*.$(ext))))
#CPPFILES != find $(SRCROOTDIR) -name "*.cpp"
#CPPFILES = $(shell find $(SRCROOTDIR) -name '*.cpp')
#CPPFILES = $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.cpp))
#CCFILES != find $(SRCROOTDIR) -name "*.cc"
#CCFILES = $(shell find $(SRCROOTDIR) -name '*.cc')
#CCFILES = $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.cc))
SRCS = $(CXXSRCS) $(CPPFILES) $(CCFILES)

_FILTER_OUT = $(foreach v,$(2),$(if $(findstring $(1),$(v)),,$(v)))
MAINSRCS = $(call _FILTER_OUT,_test.,$(SRCS))
MAINOBJS = $(filter %.o,$(foreach ext,$(CPPEXT),$(MAINSRCS:%.$(ext)=$(OBJDIR)/%.o)))
MAINDEPS = $(MAINOBJS:%o=%d)



###########################
# EXTERNAL: 3rd party libraries
###########################
#==========================
# GOOGLETEST
GTEST := gtest
GTESTINC := -Iexternal/googletest/include 
GTESTLIB := -Lexternal/googletest -lgtest_main -lgtest
# excludes other main() objects for gtest_main
GTESTOBJS = $(filter-out %main.o,$(filter %.o,\
            $(foreach ext,$(CPPEXT),$(SRCS:%.$(ext)=$(OBJDIR)/%.o))))
GTESTDEPS = $(GTESTOBJS:%o=%d)

#==========================
# SPDLOG
SPDLOGINC := -Iexternal/spdlog/1.8.2/include

#==========================
# JSON
NLOHMANNJSONINC := -Iexternal/json/nlohmannjson



###########################
# COMPILE ENV
###########################
DEBUG := yes

CXXFLAGS = -Wfatal-errors -W -Wall -MMD -pthread -std=c++17
CXXFLAGS_ROBUST = $(CXXFLAGS) -pedantic-errors -Wextra -Werror
#CXXFLAGS += -ffile-prefix-map=$(abspath $(PROJECT_ROOT))=.
ifeq ($(strip $(DEBUG)),yes)
CXXFLAGS += -Og
else
CXXFLAGS += -O
endif

#CPPDEFS += -D__DEPRECATED
ifeq ($(strip $(DEBUG)),yes)
CPPDEFS += -DDEBUG
endif
#CPPFLAGS += $(addprefix -D,$(CPPDEFS))

INCLUDES = $(SRCINC) $(NLOHMANNJSONINC) $(SPDLOGINC)
#CPPFLAGS += $(addprefix -I,$(INCLUDES))

LDFLAGS = -lstdc++ -lm
LDFLAGS += -Wl,-rpath,'$$ORIGIN'/../lib
#LDFLAGS  = $(addprefix -L,$(LIB_DIRS))
ifeq ($(strip $(DEBUG)),yes)
LDFLAGS += -static-libgcc -static-libstdc++
endif

#ARFLAGS = rvUT
#LINT.cc := cpplint --quiet



###########################
# COMMON VARIABLE
###########################
# These variables are used as arguments of the function
comma_ :=,
space_ := 
define newline_


endef

# for a warning color text in echo messagea 
tptnorm_ := tput sgr0
tptred_ := tput setaf 9 && tput bold
tptgreen_ := tput setaf 10 && tput bold
tptyellow_:= tput setaf 11 && tput bold
tptblue_ := tput setaf 12 && tput bold
tptpurple_ := tput setaf 13 && tput bold
tptcyan_ := tput setaf 14 && tput bold


###########################
# PHONY RECIPES
###########################
.DEFAULT_GOAL=all

.PHONY: all
all: main

.PHONY: main
main: buildir $(APPDIR)/$(EXECUTABLE)

.PHONY: buildir
buildir:
	@mkdir -p $(APPDIR)
	@mkdir -p $(OBJDIR)

.PHONY: clean
clean:
	-@rm -rvf $(BUILDDIR)

.PHONY: run
run: all
	@$(APPDIR)/$(EXECUTABLE)

.PHONY: lint
lint:
	@cpplint --recursive --exclude=external $(PROJECT_ROOT)


#==========================
# GTEST
.PHONY: gtest
gtest: INCLUDES += $(GTESTINC)
gtest: LDFLAGS += $(GTESTLIB)
gtest: buildir $(APPDIR)/$(GTEST)

.PHONY: gtest.list
gtest.list: gtest
	@$(APPDIR)/$(GTEST) --gtest_list_tests;

# ex) make gtest.run
# ex) make gtest.run 'testgroup.testname'
# ex) make gtest.run '*.testname'
# ex) make -- gtest.run '*.testname'
.PHONY: gtest.run
ifeq (gtest.run,$(firstword $(MAKECMDGOALS)))
RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
$(eval $(RUN_ARGS):;@:)
endif
gtest.run::
	@echo '**** GTEST START ******************************'
gtest.run:: gtest $(info $(MAKECMDGOALS))
	@if [ "$(RUN_ARGS)" ]; then \
		$(APPDIR)/$(GTEST) --gtest_filter=$(RUN_ARGS); \
	else \
		$(APPDIR)/$(GTEST); \
	fi
gtest.run::
	@echo '**** GTEST END ********************************'



############################
# TARGETS EXAMPLE
############################
.PHONY: forexample
forexample:
	@for dir in $(SRCDIRS); do \
		if [[ $$dir ]]; then echo $$dir; else echo empty...; fi \
	done



############################
# PATTERN RULES
############################
$(OBJDIR)/%.o: %.cc
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(CPPDEFS) $(INCLUDES) -o $@ -c $<

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(CPPDEFS) $(INCLUDES) -o $@ -c $<

$(APPDIR)/$(EXECUTABLE): $(MAINOBJS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(CPPDEFS) $(INCLUDES) -o $@ $(MAINOBJS) $(LDFLAGS)

$(APPDIR)/$(GTEST): $(GTESTOBJS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(CPPDEFS) $(INCLUDES) -o $@ $(GTESTOBJS) $(LDFLAGS)



############################
# FOR MAKE DEBUG
#
# Just checks value of variable
#
# USAGE: $ make print-<VARIABLE NAME>
# EX: $ make print-SRCS
############################
print-%:;$(info $* is a $(flavor $*) variable set to [$($*)])@:



############################
# TARGETS HAVE NO RULES BUT NO NEED TOBE ERROR
############################
#==========================
# Prevent to "No rule to make target" ERROR
# When header file specified in .d is removed from FS.
# HDREXT=h hh
define _NOT_TO_MAKE_EXT
%.$(1):;@echo `$$(tptred_)`warning`$$(tptnorm_)`: \($$@\) file is missing
endef
$(foreach ext, $(HDREXT), $(eval $(call _NOT_TO_MAKE_EXT,$(ext))))

define _NOT_TO_MAKE
$1:;@echo `$$(tptred_)`warning`$$(tptnorm_)`: DO NOT make \($$@\)
endef
# PREFIX=foo bar zoo
# $(foreach e, $(PREFIX), $(eval $(call _NOT_TO_MAKE,$(e)%)))
# SUFFIX=.x .y .z
# $(foreach e, $(SUFFIX), $(eval $(call _NOT_TO_MAKE,%$(e))))
# FORBIDDEN_TARGET=forbidden
# $(foreach e, $(FORBIDDEN_TARGET), $(eval $(call _NOT_TO_MAKE,$(e))))

#==========================
# % and .DEFAULT match with All targets that have no rule.
# %:;@echo `$$(tptred_)`warning`$$(tptnorm_)`: TARGET\($$@\) HAS NO RULES
# .DEFAULT:;@echo `$$(tptred_)`warning`$$(tptnorm_)`: TARGET HAS NO RULES ...



############################
# Include DEPS for header incremental build
############################
-include $(MAINDEPS) $(GTESTDEPS)
