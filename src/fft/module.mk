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
# Makefile for fft module
#
module_name	:= fft
base_dir	:= src/$(module_name)

# local object files
# 
# This is a list of local object files; dependencies are
# described below
local_objects :=				\
	$(base_dir)/src/asgram.o		\
	$(base_dir)/src/dct.o			\
	$(base_dir)/src/fftf.o			\
	$(base_dir)/src/fft_common.o		\
	$(base_dir)/src/mdct.o			\


# 
# list explicit targets and dependencies here
#

fft_includes :=					\
	$(base_dir)/src/fft_create_plan.c	\
	$(base_dir)/src/fct_execute.c		\
	$(base_dir)/src/dct_execute.c		\
	$(base_dir)/src/dst_execute.c		\
	$(base_dir)/src/mdct_execute.c		\
	$(base_dir)/src/imdct_execute.c		\
	$(base_dir)/src/fft_execute.c		\
	$(base_dir)/src/fft.common.c		\


$(base_dir)/src/fftf.o : %.o : %.c $(headers) $(fft_includes)

$(base_dir)/src/asgram.o : %.o : %.c $(headers)

$(base_dir)/src/dct.o : %.o : %.c $(headers)

$(base_dir)/src/fftf.o : %.o : %.c $(headers)

$(base_dir)/src/fft_common.o : %.o : %.c $(headers)

$(base_dir)/src/mdct.o : %.o : %.c $(headers)



# local_tests
#
# This is a list of local autotest scripts (header files) which
# are used to generate the autotest program with the 'check'
# target.  These files are located under the tests/ subdirectory
local_autotests :=					\
	$(base_dir)/tests/fft_autotest.h		\
	$(base_dir)/tests/fft_r2r_autotest.h		\
	$(base_dir)/tests/fft_shift_autotest.h		\


# local_benchmarks
#
# This is a list of local benchmark scripts which are used to
# generate the benchmark program with the 'bench' target.
# These files are located under the bench/ subdirectory
local_benchmarks :=					\
	$(base_dir)/bench/mdct_benchmark.h		\
	$(base_dir)/bench/fft_benchmark.h		\
	$(base_dir)/bench/fft_r2r_benchmark.h		\


# Build the local library and local object files
local_library	:= lib$(module_name).a
$(local_library): $(local_objects)
	$(AR) $(ARFLAGS) $@ $^

# accumulate targets
objects			+= $(local_objects)
libraries		+= $(local_library)
autotest_headers	+= $(local_autotests)
benchmark_headers	+= $(local_benchmarks)

