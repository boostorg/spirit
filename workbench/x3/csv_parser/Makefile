SHELL := /bin/bash
PATH := $(HOME)/bin:$(PATH)
HOW=clangxx3_9_pkg
ifdef USE_MACRO_INCLUDE
#invoke make with something like:
#  make USE_MACRO_INCLUDE=1
#to enable this branch of ifdef.
include MACRO_INCLUDE.imk
else
DIR.root := $(shell dirup_dir_file.pl $(PWD) root.imk)
include $(DIR.root)/root.imk
#^defines(among others):
#  nonius.inc

MACRO_INCLUDE.imk:
	echo "#Generated from Makefile with target:$@.">$@
	echo "#  Change values as appropriate for your environment.">>$@
	echo "COMPILER.$(HOW):=$(COMPILER.$(HOW))">>$@
	echo "boost.root:=$(boost.root)">>$@
	echo "nonius.inc:=$(nonius.inc)">>$@
	echo "boost.spirit.fork:=$(boost.spirit.fork)">>$@
endif
LINK.opts=-L /lib64 -l pthread
#The above from:
#  http://stackoverflow.com/questions/25617839/undefined-reference-to-symbol-pthread-key-deleteglibc-2-2-5
#to solve problem when previous version of this Makefile run.
INCS:= \
 -I$(boost.root) \
 -I$(nonius.inc) \
  $(INCS) \
  ###

.PHONY: show help compiler runs means

show:
	@echo $(HOME)
help:
	$(COMPILE.$(HOW)) -help

compiler:
	$(COMPILER.$(HOW)) --version

OPTS.optim=-O3
#OPTS.optim=-g
#-DBOOST_SPIRIT_X3_DEBUG
OPTS.spirit=$(INCS)
OPTS.fork=-I$(boost.spirit.fork) $(INCS)

define bench_make
nonius.$(1).$(2).GET$(3).XRULE$(4).run.exe: nonius.cpp $(1).hpp input.hpp
	$(COMPILER.$(HOW)) -std=c++1z $(OPTS.optim) \
	  -DBENCH_INC=$(1).hpp \
	  -DBOOST_SPIRIT_GET_RHS_CRTP=$(3) \
	  -DBOOST_SPIRIT_ATTR_XFORM_IN_RULE=$(4) \
	  $(OPTS.$(2)) $(LINK.opts) nonius.cpp -o $$@
nonius.$(1).$(2).GET$(3).XRULE$(4).run.out: nonius.$(1).$(2).GET$(3).XRULE$(4).run.exe
	./$$< -o $$@
	cat $$@
profiled.$(1).$(2).GET$(3).XRULE$(4).prof.exe: profiled.cpp $(1).hpp input.hpp
	$(COMPILER.$(HOW)) -std=c++1z -pg -O0 \
	  -DBENCH_INC=$(1).hpp \
	  -DBOOST_SPIRIT_GET_RHS_CRTP=$(3) \
	  -DBOOST_SPIRIT_ATTR_XFORM_IN_RULE=$(4) \
	  $(OPTS.$(2)) $(LINK.opts) profiled.cpp -o $$@
profiled.$(1).$(2).GET$(3).XRULE$(4).gmon.out: profiled.$(1).$(2).GET$(3).XRULE$(4).prof.exe
	./$$<
	mv gmon.out profiled.$(1).$(2).GET$(3).XRULE$(4).gmon.out
profiled.$(1).$(2).GET$(3).XRULE$(4).prof.out: profiled.$(1).$(2).GET$(3).XRULE$(4).gmon.out
	gprof --demangle profiled.$(1).$(2).GET$(3).XRULE$(4).prof.exe ./$$< > $$@
endef

BENCH_INCS:=qi x3_ns_rdef x3_crtp
VERSIONS:=spirit fork
GET_RHS:=0 1
XFORM_IN_RULE:=0 1
$(foreach inc,$(BENCH_INCS),\
$(foreach ver,$(VERSIONS),\
$(foreach get,$(GET_RHS),\
$(foreach xrule,$(XFORM_IN_RULE),\
$(eval $(call bench_make,$(inc),$(ver),$(get),$(xrule)))))))

RUNS=\
  nonius.qi.spirit.GET0.XRULE0.run.out\
  nonius.x3_ns_rdef.spirit.GET0.XRULE0.run.out\
  nonius.x3_ns_rdef.fork.GET0.XRULE0.run.out\
  nonius.x3_ns_rdef.fork.GET0.XRULE1.run.out\
  nonius.x3_ns_rdef.fork.GET1.XRULE0.run.out\
  nonius.x3_ns_rdef.fork.GET1.XRULE1.run.out\
  nonius.x3_crtp.fork.GET1.XRULE0.run.out\
  nonius.x3_crtp.fork.GET1.XRULE1.run.out\
  ###
RUNS= \
  nonius.qi.spirit.GET0.XRULE0.run.out\
  nonius.x3_ns_rdef.spirit.GET0.XRULE0.run.out\
  nonius.x3_ns_rdef.fork.GET1.XRULE0.run.out \
  nonius.x3_ns_rdef.fork.GET1.XRULE1.run.out \
  nonius.x3_crtp.fork.GET1.XRULE1.run.out \
  ###
runs: $(RUNS)

PROFS=\
  profiled.x3_ns_rdef.fork.GET1.XRULE0.prof.out\
#  nonius.x3_ns_rdef.fork.GET1.XRULE0.prof.out\
  nonius.x3_ns_rdef.fork.GET1.XRULE1.prof.out\
  ###
profs: $(PROFS)

means: $(RUNS)
	grep -e '^mean: ' nonius.*.run.out > means.out
	cat means.out

test:
	$(COMPILER.$(HOW)) -DBENCH_INC=test.hpp -std=c++1z -pg test.cpp -o test.exe
	./test.exe
	#gprof test.exe > $@.gprof_out

prof_grep:
	perl filt_gprof_flat.pl < nonius.x3_ns_rdef.spirit.GET0.XRULE0.prof.out



