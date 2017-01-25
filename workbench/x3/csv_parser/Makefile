SHELL := /bin/bash
PATH := $(USER)/bin:$(PATH)
HOW=clangxx3_9_pkg
ifdef USE_MACRO_INCLUDE
#invoke make with something like:
#  make USE_MACRO_INCLUDE=1
#to enable this branch of ifdef.
include MACRO_INCLUDE.imk
else
DIR.root := $(shell dirup_dir_file.pl $(PWD) root.imk)
include $(DIR.root)/root.imk

MACRO_INCLUDE.imk:
	echo "#Generated from Makefile with target:$@.">$@
	echo "#  Change values as appropriate for your environment.">>$@
	echo "COMPILER.$(HOW):=$(COMPILER.$(HOW))">>$@
	echo "boost.root:=$(boost.root)">>$@
	echo "nonius:=$(nonius)">>$@
	echo "boost.spirit.fork:=$(boost.spirit.fork)">>$@
endif
LINK.opts=-L /lib64 -l pthread
#The above from:
#  http://stackoverflow.com/questions/25617839/undefined-reference-to-symbol-pthread-key-deleteglibc-2-2-5
#to solve problem when previous version of this Makefile run.
INCS:= \
 -I$(boost.root) \
 -I$(nonius) \
  $(INCS) \
  ###

.PHONY: show help compiler runs means

show:
	@echo $(INCS)
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
$(1).$(2).GET$(3).XRULE$(4).exe: $(1).cpp
	$(COMPILER.$(HOW)) -std=c++1z $(OPTS.optim) \
	  -DBENCH_NAME="$(1).$(2)" \
	  -DBOOST_SPIRIT_GET_RHS_CRTP=$(3) \
	  -DBOOST_SPIRIT_ATTR_XFORM_IN_RULE=$(4) \
	  $(OPTS.$(2)) $(LINK.opts) $(1).cpp -o $$@
$(1).$(2).GET$(3).XRULE$(4).run: $(1).$(2).GET$(3).XRULE$(4).exe
	./$$< -o $$@
	cat $$@
endef

BENCH_MAINS:=main_x3_ns main_x3_crtp main_qi
VERSIONS:=spirit fork
GET_RHS:=0 1
XFORM_IN_RULE:=0 1
$(foreach main,$(BENCH_MAINS),\
$(foreach ver,$(VERSIONS),\
$(foreach get,$(GET_RHS),\
$(foreach xrule,$(XFORM_IN_RULE),\
$(eval $(call bench_make,$(main),$(ver),$(get),$(xrule)))))))

RUNS=\
  main_qi.spirit.GET0.XRULE0.run\
  main_x3_ns.spirit.GET0.XRULE0.run\
  main_x3_ns.fork.GET0.XRULE0.run\
  main_x3_ns.fork.GET0.XRULE1.run\
  main_x3_ns.fork.GET1.XRULE0.run\
  main_x3_ns.fork.GET1.XRULE1.run\
  main_x3_crtp.fork.GET1.XRULE0.run\
  main_x3_crtp.fork.GET1.XRULE1.run\
  ###
runs: $(RUNS)

means: $(RUNS)
	grep -e '^mean: ' main_*.run > means.out
	cat means.out

