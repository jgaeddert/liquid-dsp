# Copyright (c) 2007, 2009, 2011 Joseph Gaeddert
# Copyright (c) 2007, 2009, 2011 Virginia Polytechnic Institute & State University
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
module_name	:= fec
base_dir	:= src/$(module_name)

# local object files
# 
# This is a list of local object files; dependencies are
# described below
local_objects :=				\
	$(base_dir)/src/crc.o			\
	$(base_dir)/src/fec.o			\
	$(base_dir)/src/fec_conv.o		\
	$(base_dir)/src/fec_conv_poly.o		\
	$(base_dir)/src/fec_conv_pmatrix.o	\
	$(base_dir)/src/fec_conv_punctured.o	\
	$(base_dir)/src/fec_hamming74.o		\
	$(base_dir)/src/fec_hamming84.o		\
	$(base_dir)/src/fec_hamming128.o	\
	$(base_dir)/src/fec_pass.o		\
	$(base_dir)/src/fec_rep3.o		\
	$(base_dir)/src/fec_rep5.o		\
	$(base_dir)/src/fec_rs.o		\


# 
# list explicit targets and dependencies here
#

$(local_objects) : %.o : %.c $(headers)



# local_tests
#
# This is a list of local autotest scripts (header files) which
# are used to generate the autotest program with the 'check'
# target.  These files are located under the tests/ subdirectory
local_autotests :=					\
	$(base_dir)/tests/crc_autotest.h		\
	$(base_dir)/tests/fec_autotest.h		\
	$(base_dir)/tests/fec_hamming74_autotest.h	\
	$(base_dir)/tests/fec_hamming84_autotest.h	\
	$(base_dir)/tests/fec_hamming128_autotest.h	\
	$(base_dir)/tests/fec_reedsolomon_autotest.h	\
	$(base_dir)/tests/fec_rep3_autotest.h		\
	$(base_dir)/tests/fec_rep5_autotest.h


# local_benchmarks
#
# This is a list of local benchmark scripts which are used to
# generate the benchmark program with the 'bench' target.
# These files are located under the bench/ subdirectory
local_benchmarks :=					\
	$(base_dir)/bench/fec_encode_benchmark.h	\
	$(base_dir)/bench/fec_decode_benchmark.h	\


# Build the local library and local object files
local_library	:= lib$(module_name).a
$(local_library): $(local_objects)
	$(AR) $(ARFLAGS) $@ $^

# accumulate targets
objects			+= $(local_objects)
libraries		+= $(local_library)
autotest_headers	+= $(local_autotests)
benchmark_headers	+= $(local_benchmarks)

