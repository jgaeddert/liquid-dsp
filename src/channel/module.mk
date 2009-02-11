# 
# Makefile for channel module
#

# local_dir: local library directory
# 
local_dir	:= src/channel
local_lib	:= libchannel.a

# local_s_files
#
# This is a list of local source files to compile into objects,
# referenced from the src/ subdirectory under $(local_dir)
#
local_s_files	:=			\
	awgn_channel.c			\
	channel.c			\
	ricek_channel.c

# local_h_files
#
# This is a list of local header files upon which the objects
# depend.  These files are also located under the src/
# subdirectory within $(local_dir)
#
local_h_files	:= channel_internal.h

# local_t_files
#
# This is a list of local autotest scripts (header files) which
# are used to generate the autotest program with the 'check'
# target.  These files are located under the tests/ subdirectory
# within $(local_dir)
#
local_t_files	:= channel_autotest.h


# local_b_files
#
# This is a list of local benchmark scripts which are used to
# generate the benchmark program with the 'bench' target.
# These files are located under the bench/ subdirectory within
# $(local_dir)
#
local_b_files	:=


# local_e_files
#
# This is a list of local example files to demonstrate
# functionality of the module.
#
local_e_files	:=			\
	fading_channel_example.c	\
	fading_generator_example.c	\
	ricek_channel_example.c

include common.mk

