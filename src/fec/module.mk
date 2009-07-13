# Copyright (c) 2007, 2009 Joseph Gaeddert
# Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
#
# This file is part of liquid.
#
# liquid is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# liquid is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with liquid.  If not, see <http://www.gnu.org/licenses/>.

# 
# Makefile for fec module
#
module_name     := fec

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
	fec_conv27.c			\
	fec_hamming74.c			\
	fec_hamming84.c			\
	fec_pass.c			\
	fec_rep.c

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


include common.mk

