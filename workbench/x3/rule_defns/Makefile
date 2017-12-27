SHELL := /bin/bash
PATH := $(USER)/bin:$(PATH)
PWD.real := `realpath $(PWD)`

MAIN=rule_defns
MAIN=rule_defns_bench
MAIN=static_assert_template
RULE2RHS_WHICH:=3
#rule_defns_bench.opts_compile.cmd:=-DRULE2RHS_WHICH=$(RULE2RHS_WHICH)
#MAIN=parse_rule_def
#MAIN=parse_rule_crtp
HOW=clangxx5_0_pkg
HOW=gcc6_20151018
ifdef USE_MACRO_INCLUDE
include MACRO_INCLUDE.imk
else
DIR.root := $(shell dirup_dir_file.pl $(PWD.real) root.imk)
include $(DIR.root)/root.imk

INCS:= -I$(nonvartmpl.root) -I$(boost.root) -DRULE2RHS_WHICH=$(RULE2RHS_WHICH) $(INCS)
.PHONY: MACRO_INCLUDE.imk bench_analyze check_bench_compiles clang_cmd gen_cmds
#.ONESHELL:

MACRO_INCLUDE.imk:
	echo "#generated from Makefile with target:$@.">$@
	echo "boost.release:=$(boost.release)">>$@
	echo "COMPILER.$(HOW):=$(COMPILER.$(HOW))">>$@
endif

help:
	$(COMPILE.$(HOW)) -help

compiler:
	$(COMPILER.$(HOW)) --version

define make_helper
$(1).exe: $(1).cpp
	$(COMPILER.$(HOW)) -std=c++14 $(1).cpp -o $(1).exe
endef
helper.exes:=RULE2RHS_METHODS gen_benchmark_gram
$(foreach helper,$(helper.exes),$(eval $(call make_helper,$(helper))))

gram_file.hpp: gen_benchmark_gram.exe 
	./gen_benchmark_gram.exe 3
	cat gram_file.hpp

rule2rhs: RULE2RHS_METHODS.exe
	./RULE2RHS_METHODS.exe $(METHODS)

BENCH_PROG=rule_defns_bench
BENCH_NOW:=bench:$(HOW).$(shell date --iso-8601=seconds).log
BENCH_TMP:=bench.tmp
CXX_STD:=--std=c++17
BENCH_EXT:=exe
BENCH_COMPILE.o:=-c
BENCH_COMPILE.exe:=
BENCH_COMPILE.cmd := $(COMPILER.$(HOW)) $(BENCH_COMPILE.$(BENCH_EXT)) $(CXX_STD) $(BENCH_PROG).cpp -o $(BENCH_PROG).$(BENCH_EXT)

NUM_RULES=8 12 14
METHODS=0 1 2 3 4 5
DELIM:="\t"
TIME.fmt:=$(DELIM)"%U"$(DELIM)"%S"$(DELIM)"%e"
GUAGECMD:=/usr/bin/time --format=$(TIME.fmt)
NUM_SAMPLES:=1 #In case want to average performance somehow, change to >1.
SAMPLE_NUMS:=$(shell seq -s ' ' 1 $(NUM_SAMPLES))
STRIP_NL:=perl -pe "chomp;"

show:
	echo "TIME.fmt=$(TIME.fmt)"
	echo "GUAGECMD=$(GUAGECMD)"
	echo "BENCH_COMPILE.cmd=$(BENCH_COMPILE.cmd)"

strip_nl:
	printf "line1.\nline2.\nline3.\n">$(BENCH_TMP)
	cat $(BENCH_TMP)
	cat $(BENCH_TMP)|$(STRIP_NL)

guage:
	echo -n "begin{">$(BENCH_TMP)
	{ $(GUAGECMD) sleep 1 ; } 2>&1 | $(STRIP_NL) >> $(BENCH_TMP)
	echo "}end" >> $(BENCH_TMP)
	cat $(BENCH_TMP)

sequence:
	echo "SAMPLE_NUMS=$(SAMPLE_NUMS)"
	printf "%s\n" "BENCH_TMP="$(BENCH_TMP)>$(BENCH_TMP)
	for sample_num in $(SAMPLE_NUMS) ;\
	do\
	  printf $(DELIM)"%s\n" "sample="$$sample_num >> $(BENCH_TMP) ;\
	done ;\
	cat $(BENCH_TMP)

check_bench_compiles: gram_file.hpp
	./gen_benchmark_gram.exe 3
	for rule2rhs_which in $(METHODS) ;\
	do \
	  echo "rule2rhs_which="$$rule2rhs_which ;\
	  $(BENCH_COMPILE.cmd) -DUSE_GRAM_FILE -DRULE2RHS_WHICH=" $$rule2rhs_which " ;\
	  ls -l $(BENCH_PROG).$(BENCH_EXT) ;\
	done

benchmark: rule_defns_bench.cpp gen_benchmark_gram.exe RULE2RHS_METHODS.exe
	./RULE2RHS_METHODS.exe $(METHODS) >$(BENCH_TMP)
	printf "%s"$(DELIM)"%s"$(DELIM)"%s"$(DELIM)"%s"$(DELIM)"%s"$(DELIM)"%s"$(DELIM)"%s"$(DELIM)"\n" "rules" "method" "sample" "user" "system" "elapsed" "size_out">>$(BENCH_TMP)
	@for num_rules in $(NUM_RULES) ;\
	do \
	  ./gen_benchmark_gram.exe $$num_rules ;\
	  for rule2rhs_which in $(METHODS) ;\
	  do \
	    for sample_num in $(SAMPLE_NUMS) ;\
	    do \
	      printf "%s" $$num_rules>>$(BENCH_TMP) ;\
	      printf $(DELIM)"%s" $$rule2rhs_which>>$(BENCH_TMP) ;\
	      printf $(DELIM)"%s" $$sample_num>>$(BENCH_TMP) ;\
	      { $(GUAGECMD) $(BENCH_COMPILE.cmd) -DUSE_GRAM_FILE -DRULE2RHS_WHICH=" $$rule2rhs_which " ; } 2>&1|$(STRIP_NL)>>$(BENCH_TMP) ;\
	      printf $(DELIM)>>$(BENCH_TMP) ;\
	      du --bytes $(BENCH_PROG).$(BENCH_EXT)|perl -lane 'print shift(@F)'>>$(BENCH_TMP) ;\
	    done ;\
	  done \
	done
	cat $(BENCH_TMP)
	#cp $(BENCH_TMP) $(BENCH_NOW)

bench_analyze:
	./benchmark_analyze.py bench_on:2016-12-28T17:24:58-06:00.out
