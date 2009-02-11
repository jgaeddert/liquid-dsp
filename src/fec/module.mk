# 
# Makefile for fec module
#

# local_dir: local library directory
# 
local_dir	:= src/fec
local_lib	:= libfec.a

# local_s_files
#
# This is a list of local source files to compile into objects,
# referenced from the src/ subdirectory under $(local_dir)
#
local_s_files	:=			\
	c_ones_mod2.c			\
	checksum.c			\
	crc.c				\
	fec.c				\
	fec_hamming74.c			\
	fec_hamming84.c			\
	fec_pass.c			\
	fec_rep.c

# local_h_files
#
# This is a list of local header files upon which the objects
# depend.  These files are also located under the src/
# subdirectory within $(local_dir)
#
local_h_files	:= fec_internal.h

# local_t_files
#
# This is a list of local autotest scripts (header files) which
# are used to generate the autotest program with the 'check'
# target.  These files are located under the tests/ subdirectory
# within $(local_dir)
#
local_t_files	:=			\
	checksum_autotest.h		\
	crc_autotest.h			\
	fec_autotest.h			\
	fec_hamming74_autotest.h	\
	fec_rep3_autotest.h


# local_b_files
#
# This is a list of local benchmark scripts which are used to
# generate the benchmark program with the 'bench' target.
# These files are located under the bench/ subdirectory within
# $(local_dir)
#
local_b_files	:=			\
	fec_encode_benchmark.h		\
	fec_decode_benchmark.h


# local_e_files
#
# This is a list of local example files to demonstrate
# functionality of the module.
#
local_e_files	:=			\
	checksum_example.c		\
	crc_example.c			\
	fec_hamming74_example.c		\
	fec_hamming84_example.c		\
	fec_rep3_example.c

include common.mk

