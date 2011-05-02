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
# Makefile for equalization module
#
module_name	:= equalization
base_dir	:= src/$(module_name)

# local object files
# 
# This is a list of local object files; dependencies are
# described below
local_objects :=				\
	$(base_dir)/src/equalizer_cccf.o	\
	$(base_dir)/src/equalizer_rrrf.o	\


# 
# list explicit targets and dependencies here
#

$(local_objects) : %.o : %.c $(headers) $(base_dir)/src/eqlms.c $(base_dir)/src/eqrls.c



# local_tests
#
# This is a list of local autotest scripts (header files) which
# are used to generate the autotest program with the 'check'
# target.  These files are located under the tests/ subdirectory
local_autotests :=					\
	$(base_dir)/tests/eqrls_rrrf_autotest.h		\


# local_benchmarks
#
# This is a list of local benchmark scripts which are used to
# generate the benchmark program with the 'bench' target.
# These files are located under the bench/ subdirectory
local_benchmarks :=					\
	$(base_dir)/bench/eqlms_cccf_benchmark.h	\
	$(base_dir)/bench/eqrls_cccf_benchmark.h	\


# Build the local library and local object files
local_library	:= lib$(module_name).a
$(local_library): $(local_objects)
	$(AR) $(ARFLAGS) $@ $^

# accumulate targets
objects			+= $(local_objects)
libraries		+= $(local_library)
autotest_headers	+= $(local_autotests)
benchmark_headers	+= $(local_benchmarks)

