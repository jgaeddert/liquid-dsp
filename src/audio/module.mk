# Copyright (c) 2007, 2008, 2009, 2010, 2011 Joseph Gaeddert
# Copyright (c) 2007, 2008, 2009, 2010, 2011 Virginia Polytechnic
#                                      Institute & State University
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
# Makefile for audio module
#
module_name	:= audio
base_dir	:= src/$(module_name)


# local object files
# 
# This is a list of local object files; dependencies are
# described below
local_objects :=			\
	src/audio/src/cvsd.o		\

# 
# list explicit targets and dependencies here
#
src/cvsd/src/cvsd.o : %.o : %.c $(headers)


# local_tests
#
# This is a list of local autotest scripts (header files) which
# are used to generate the autotest program with the 'check'
# target.  These files are located under the tests/ subdirectory
local_autotests :=			\
	src/audio/tests/cvsd_autotest.h


# local_benchmarks
#
# This is a list of local benchmark scripts which are used to
# generate the benchmark program with the 'bench' target.
# These files are located under the bench/ subdirectory
local_benchmarks :=


# Build the local library and local object files
local_library	:= lib$(module_name).a
$(local_library): $(local_objects)
	$(AR) $(ARFLAGS) $@ $^

# accumulate targets
objects			+= $(local_objects)
libraries		+= $(local_library)
autotest_headers	+= $(local_autotests)
benchmark_headers	+= $(local_benchmarks)

