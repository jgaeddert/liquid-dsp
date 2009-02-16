# 
# Makefile for buffer module
#

# local_dir: local library directory
# 
local_dir	:= src/buffer
local_lib	:= libbuffer.a

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

# local_h_files
#
# This is a list of local header files upon which the objects
# depend.  These files are also located under the src/
# subdirectory within $(local_dir)
#
local_h_files	:= buffer_internal.h

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
	gport2_autotest.h


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
	gport_threaded_benchmark.h

include common.mk

