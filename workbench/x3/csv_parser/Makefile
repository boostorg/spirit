#Purpose:
#  1) Benchmark various methods to implement rule recursion.
#  2) Use the results of 1) to help diagnose why there's the
#     run-time slow down with x3 vs qi csv parser as reported here:
#       http://boost.2283326.n4.nabble.com/Design-structure-X3-parser-more-like-Qi-parser-tp4690205p4690362.html
#=================
SHELL := /bin/bash
PATH := $(HOME)/bin:$(PATH)
REALPWD := $(shell realpath $(PWD))
#Specify compiler to use:
  HOW=clangxx5_0_pkg
  #HOW=gcc6_20151018
#Specify the grammar complexity and corresponding test data
  COMPLEXITY=complex
  #COMPLEXITY=simple
#Specify the grammar to use:
  GRAM=csv_$(COMPLEXITY)
#Specify the test data
  TEST_DATA=csv_$(COMPLEXITY)
ifdef USE_MACRO_INCLUDE
#invoke make with something like:
#  make USE_MACRO_INCLUDE=1
#to enable this branch of ifdef.
include MACRO_INCLUDE.imk
else
DIR.root := $(shell dirup_dir_file.pl $(REALPWD) root.imk)
include $(DIR.root)/root.imk
#^defines(among others):
#  nonius.inc

MACRO_INCLUDE.imk:
	echo "#Generated from Makefile with target:$@.">$@
	echo "#  Change values as appropriate for your environment.">>$@
	echo "COMPILER.$(HOW):=$(COMPILER.$(HOW))">>$@
	echo "boost.root:=$(boost.root)">>$@
	echo "nonius.inc:=$(nonius.inc)">>$@
	echo "#^See https://github.com/rmartinho/nonius.git">>$@
	echo "pprof.install:=$(pprof.install)">>$@
	echo "#^Built from https://github.com/gperftools/gperftools.git">>$@
	echo "boost.spirit.fork:=$(boost.spirit.fork)">>$@
	echo "#^develop branch of https://github.com/cppljevans/spirit.git">>$@
endif
LINK.nonius.opts=-L /lib64 -l pthread
  #The above from:
  #  http://stackoverflow.com/questions/25617839/undefined-reference-to-symbol-pthread-key-deleteglibc-2-2-5
  #to solve problem when previous version of this Makefile run.
  ###
COMPILE.gprof=-pg -g -static-libgcc -static-libstdc++
  #In the above:
  #  The -pg is required by:
  #    https://sourceware.org/binutils/docs/gprof/Compiling.html#Compiling    
  #  The -static-lib* is justified by:
  #      https://sourceware.org/binutils/docs/gprof/Compiling.html#Compiling
  #    to avoid empty output when gprof run with the --annotated-source
  #    and 0.0 times otherwise.
  #  The -g is to allow --annotated-source to work as explained here:
  #    https://sourceware.org/ml/binutils/2008-12/msg00068.html
  ###
INCS:= \
 -I$(boost.root) \
  $(INCS) \
  ###The nonius.inc is only useful for nonius.cpp

#info PHONIES: phonies just providing information:
.PHONY: show help compiler
#target PHONIES: phonies which actually create something:
.PHONY: test flat_graphs annots pprofs runs profs gcovs gcovs.force
.PHONY: benchs.chrono summary.chrono benchs.nonius summary.nonius

help:
	$(COMPILE.$(HOW)) -help

compiler:
	$(COMPILER.$(HOW)) --version

OPTS.spirit=-I$(boost.root) #use normal spirit library
OPTS.fork=-I$(boost.spirit.fork) $(OPTS.spirit) #use fork library in preference to normal spirit library.
target_deps=$(1)-$(GRAM).hpp input-$(GRAM).hpp bench_run.hpp trace_attr-csv.hpp
target_repeat_deps=$(target_deps) bench_repeat.hpp ITERATIONS.hpp

show:
	@echo $(target_deps)

define targets_gen
#  Generate various targets dependent on parameters:
#    $(1) : the include file prefix for rule recursion method.
#    $(2) : -I flags for spirit fork to use.
#    $(3) : macro BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_CRTP value.
#    $(4) : macro BOOST_SPIRIT_X3_EXPERIMENTAL_ATTR_XFORM_IN_RULE value.
#    $(5) : other compile flags (e.g. optimization level).

nonius-$(GRAM)-$(1)-$(2)-GET$(3)-XRULE$(4)-Opt$(5)-$(HOW).exe: nonius.cpp $(target_deps)
	$(COMPILER.$(HOW)) -std=c++1z -$(5) \
	  -DTEST_DATA=input-$(TEST_DATA).hpp \
	  -DHOW_INC=$(1)-$(GRAM).hpp \
	  -DBOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_CRTP=$(3) \
	  -DBOOST_SPIRIT_X3_EXPERIMENTAL_ATTR_XFORM_IN_RULE=$(4) \
	  $(OPTS.$(2)) -I$(nonius.inc) $(LINK.nonius.opts) $$< -o $$@
nonius-$(GRAM)-$(1)-$(2)-GET$(3)-XRULE$(4)-Opt$(5)-$(HOW).out: nonius-$(GRAM)-$(1)-$(2)-GET$(3)-XRULE$(4)-Opt$(5)-$(HOW).exe
	./$$< -o $$@
	cat $$@
chrono-$(GRAM)-$(1)-$(2)-GET$(3)-XRULE$(4)-Opt$(5)-$(HOW).exe: chrono.cpp $(target_repeat_deps)
	$(COMPILER.$(HOW)) -std=c++1z -$(5) \
	  -DTEST_DATA=input-$(TEST_DATA).hpp \
	  -DHOW_INC=$(1)-$(GRAM).hpp \
	  -DBOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_CRTP=$(3) \
	  -DBOOST_SPIRIT_X3_EXPERIMENTAL_ATTR_XFORM_IN_RULE=$(4) \
	  $(OPTS.$(2)) $$< -o $$@
	@echo $(target_deps)
chrono-$(GRAM)-$(1)-$(2)-GET$(3)-XRULE$(4)-Opt$(5)-$(HOW).out: chrono-$(GRAM)-$(1)-$(2)-GET$(3)-XRULE$(4)-Opt$(5)-$(HOW).exe
	./$$< > $$@
	cat $$@
gprof-$(GRAM)-$(1)-$(2)-GET$(3)-XRULE$(4)-Opt$(5)-$(HOW).exe: bench_repeat.cpp $(target_repeat_deps)
	$(COMPILER.$(HOW)) -std=c++1z $(COMPILE.gprof) -$(5) \
	  -DHOW_INC=$(1)-$(GRAM).hpp \
	  -DBOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_CRTP=$(3) \
	  -DBOOST_SPIRIT_X3_EXPERIMENTAL_ATTR_XFORM_IN_RULE=$(4) \
	  $(OPTS.$(2)) $$< -o $$@
gprof-$(GRAM)-$(1)-$(2)-GET$(3)-XRULE$(4)-Opt$(5)-$(HOW).gmon: gprof-$(GRAM)-$(1)-$(2)-GET$(3)-XRULE$(4)-Opt$(5)-$(HOW).exe
	./$$<
	mv gmon.out $$@
gprof-$(GRAM)-$(1)-$(2)-GET$(3)-XRULE$(4)-Opt$(5)-$(HOW).annot: gprof-$(GRAM)-$(1)-$(2)-GET$(3)-XRULE$(4)-Opt$(5)-$(HOW).gmon
	gprof --demangle --line --annotated-source -x gprof-$(GRAM)-$(1)-$(2)-GET$(3)-XRULE$(4)-Opt$(5)-$(HOW).exe $$< | c++filt > $$@
gprof-$(GRAM)-$(1)-$(2)-GET$(3)-XRULE$(4)-Opt$(5)-$(HOW).flat_graph: gprof-$(GRAM)-$(1)-$(2)-GET$(3)-XRULE$(4)-Opt$(5)-$(HOW).gmon
	gprof --demangle --line gprof-$(GRAM)-$(1)-$(2)-GET$(3)-XRULE$(4)-Opt$(5)-$(HOW).exe $$< | c++filt > $$@
pprof-$(GRAM)-$(1)-$(2)-GET$(3)-XRULE$(4)-Opt$(5)-$(HOW).exe: pprof.cpp $(target_repeat_deps)
	$(COMPILER.$(HOW)) -std=c++1z -$(5) \
	  -DHOW_INC=$(1)-$(GRAM).hpp \
	  -DBOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_CRTP=$(3) \
	  -DBOOST_SPIRIT_X3_EXPERIMENTAL_ATTR_XFORM_IN_RULE=$(4) \
	  -I$(pprof.install)/include \
	  -L $(pprof.install)/lib -l profiler \
	  $(OPTS.$(2)) $$< -o $$@
pprof-$(GRAM)-$(1)-$(2)-GET$(3)-XRULE$(4)-Opt$(5)-$(HOW).prof: pprof-$(GRAM)-$(1)-$(2)-GET$(3)-XRULE$(4)-Opt$(5)-$(HOW).exe
	  ./$$< $$@
pprof-$(GRAM)-$(1)-$(2)-GET$(3)-XRULE$(4)-Opt$(5)-$(HOW).text: \
  pprof-$(GRAM)-$(1)-$(2)-GET$(3)-XRULE$(4)-Opt$(5)-$(HOW).exe\
  pprof-$(GRAM)-$(1)-$(2)-GET$(3)-XRULE$(4)-Opt$(5)-$(HOW).prof
	  $(pprof.install)/bin/pprof --text $$^ > $$@
gcov-$(GRAM)-$(1)-$(2)-GET$(3)-XRULE$(4)-Opt$(5)-$(HOW).dir: bench_repeat.cpp gcovs.force $(target_repeat_deps)
	#***NOTE*** gcov only works with gcc (according to man page).
	mkdir -p $$@
	-rm $$@/*.gcov $$@/bench_repeat.gcda $$@/bench_repeat.gcno
	cd $$@ ; \
	$(COMPILER.$(HOW)) -std=c++1z -fprofile-arcs -ftest-coverage -$(5) \
	  -DHOW_INC=$(1)-$(GRAM).hpp \
	  -DBOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_CRTP=$(3) \
	  -DBOOST_SPIRIT_X3_EXPERIMENTAL_ATTR_XFORM_IN_RULE=$(4) \
	  $(OPTS.$(2)) ../$$< -o bench_repeat.exe; \
	./bench_repeat.exe ; \
	gcov --demangled-names --object-file bench_repeat.exe ../$$< ; \
	rm bench_repeat.exe
endef

#target_gen parameters:
  HOW_INC:=qi x3_ns_rdef x3_crtp #filename of #include for rule definition method.
  #  There are 3 "general" rule definition methods:
  #    * qi: use qi to define rules.
  #    * x3_ns_rdef:
  #      use:
  #        auto x_def = x = x_rhs;
  #      where:
  #        rule<x_id,x_attr> x;
  #      for some types, x_id and x_attr, and some grammar expression, x_rhs,
  #      to define rules.
  #    * x3_crtp: use Curiously Recurring Template Pattern
  #      to define rules.
  #      
  FORKS:=spirit fork
  GET_RHS:=0 1
  XFORM_IN_RULE:=0 1
  OPTIM:=O0 O1 O2 O3

#Now generate targets for all combinations of above target parameters:
$(foreach inc,$(HOW_INC),\
$(foreach ver,$(FORKS),\
$(foreach get,$(GET_RHS),\
$(foreach xrule,$(XFORM_IN_RULE),\
$(foreach optim,$(OPTIM),\
$(eval $(call targets_gen,$(inc),$(ver),$(get),$(xrule),$(optim))))))))

#Specify which targets are actually created:
BENCHS.stem=\
  $(GRAM)-x3_ns_rdef-fork-GET1-XRULE0-OptO0-$(HOW)\
  $(GRAM)-x3_ns_rdef-fork-GET1-XRULE0-OptO2-$(HOW)\
  $(GRAM)-qi-fork-GET1-XRULE0-OptO0-$(HOW)\
  $(GRAM)-qi-fork-GET1-XRULE0-OptO2-$(HOW)\
  ###
#BENCHS.short=yes
ifdef BENCHS.short
BENCHS.stem=\
  $(GRAM)-qi-spirit-GET0-XRULE1-OptO0-$(HOW)\
  $(GRAM)-qi-spirit-GET1-XRULE0-OptO0-$(HOW)\
  ###
endif

BENCHS.nonius=$(addprefix nonius-,$(addsuffix .out,$(BENCHS.stem)))
benchs.nonius: $(BENCHS.nonius)

summary.nonius: benchs.nonius
	grep -e '^mean: ' nonius-$(GRAM)-*-$(HOW).out > summary.nonius-$(GRAM)-$(HOW).out
	cat summary.nonius-$(GRAM)-$(HOW).out

BENCHS.chrono=$(addprefix chrono-,$(addsuffix .out,$(BENCHS.stem)))
  ###
benchs.chrono: $(BENCHS.chrono)

summary.chrono: benchs.chrono
	grep -e '^mean: ' chrono-$(GRAM)-*-$(HOW).out > summary.chrono-$(GRAM)-$(HOW).out
	cat summary.chrono-$(GRAM)-$(HOW).out

PROFS.stem=\
  $(GRAM)-qi-spirit-GET0-XRULE0-OptO0-$(HOW)\
  $(GRAM)-qi-spirit-GET0-XRULE0-OptO1-$(HOW)\
  $(GRAM)-x3_ns_rdef-spirit-GET0-XRULE0-OptO0-$(HOW)\
  $(GRAM)-x3_ns_rdef-spirit-GET0-XRULE0-OptO1-$(HOW)\
  ###
PROFS.stem=\
  $(GRAM)-x3_ns_rdef-spirit-GET0-XRULE0-OptO1-$(HOW)\
  ###

FLAT_GRAPHS=$(addprefix gprof-,$(addsuffix .flat_graph,$(PROFS.stem)))
flat_graphs: $(FLAT_GRAPHS)

GCOVS=$(addprefix gcov-,$(addsuffix .dir,$(PROFS.stem)))
gcovs: $(GCOVS)

ANNOTS=$(addprefix gprof-,$(addsuffix .annot,$(PROFS.stem)))
annots: $(ANNOTS)

PPROFS=$(addprefix pprof-,$(addsuffix .text,$(PROFS.stem)))
pprofs: $(PPROFS)

PROFS=\
  profiled.x3_ns_rdef.fork.GET1.XRULE0.prof.out\
#  nonius.x3_ns_rdef.fork.GET1.XRULE0.prof.out\
  nonius.x3_ns_rdef.fork.GET1.XRULE1.prof.out\
  ###
profs: $(PROFS)

test:
	$(COMPILER.$(HOW)) -DHOW_INC=test.hpp -std=c++1z -pg test.cpp -o test.exe
	./test.exe
	#gprof test.exe > $@.gprof_out

prof_grep:
	perl filt_gprof_flat.pl < nonius-$(GRAM)-x3_ns_rdef.spirit.GET0.XRULE0-OptO0-$(HOW).gprof.out
