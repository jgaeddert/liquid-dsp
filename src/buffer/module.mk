# 
# Makefile for buffer module
#
module_name     := buffer

# local_s_files
#
# This is a list of local source files to compile into objects,
# referenced from the src/ subdirectory under $(local_dir)
#
local_s_files	:= 	\
	fbuffer.c	\
	cfbuffer.c	\
	uibuffer.c	\
	gport.c		\
	gport2.c

# local_t_files
#
# This is a list of local autotest scripts (header files) which
# are used to generate the autotest program with the 'check'
# target.  These files are located under the tests/ subdirectory
# within $(local_dir)
#
local_t_files	:= 		\
	cbuffer_autotest.h	\
	sbuffer_autotest.h	\
	gport2_autotest.h	\
	window_autotest.h


# local_b_files
#
# This is a list of local benchmark scripts which are used to
# generate the benchmark program with the 'bench' target.
# These files are located under the bench/ subdirectory within
# $(local_dir)
#
local_b_files	:= 			\
	gport2_benchmark.h		\
	gport2_threaded_benchmark.h	\
	gport_benchmark.h		\
	gport_threaded_benchmark.h	\
	window_benchmark.h

include common.mk

