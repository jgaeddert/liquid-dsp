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
# Makefile for filter module
#
module_name	:= filter
base_dir	:= src/$(module_name)

# local object files
# 
# This is a list of local object files; dependencies are
# described below
local_objects :=			\
	$(base_dir)/src/bessel.o	\
	$(base_dir)/src/butter.o	\
	$(base_dir)/src/cheby1.o	\
	$(base_dir)/src/cheby2.o	\
	$(base_dir)/src/ellip.o		\
	$(base_dir)/src/filter_rrrf.o	\
	$(base_dir)/src/filter_crcf.o	\
	$(base_dir)/src/filter_cccf.o	\
	$(base_dir)/src/firdes.o	\
	$(base_dir)/src/firdespm.o	\
	$(base_dir)/src/group_delay.o	\
	$(base_dir)/src/hM3.o		\
	$(base_dir)/src/iirdes.pll.o	\
	$(base_dir)/src/iirdes.o	\
	$(base_dir)/src/lpc.o		\
	$(base_dir)/src/rcos.o		\
	$(base_dir)/src/rkaiser.o	\
	$(base_dir)/src/rrcos.o		\

#	$(base_dir)/src/nyquist.o


# 
# list explicit targets and dependencies here
#

filter_includes :=			\
	$(base_dir)/src/firfarrow.c	\
	$(base_dir)/src/firfilt.c	\
	$(base_dir)/src/firpfb.c	\
	$(base_dir)/src/iirfilt.c	\
	$(base_dir)/src/iirfiltsos.c	\
	$(base_dir)/src/interp.c	\
	$(base_dir)/src/decim.c		\
	$(base_dir)/src/resamp.c	\
	$(base_dir)/src/resamp2.c	\
	$(base_dir)/src/symsync.c	\

$(base_dir)/src/bessel.o : %.o : %.c $(headers)

$(base_dir)/src/bessel.o : %.o : %.c $(headers)

$(base_dir)/src/butter.o : %.o : %.c $(headers)

$(base_dir)/src/cheby1.o : %.o : %.c $(headers)

$(base_dir)/src/cheby2.o : %.o : %.c $(headers)

$(base_dir)/src/ellip.o : %.o : %.c $(headers)

$(base_dir)/src/filter_rrrf.o : %.o : %.c $(headers) $(filter_includes)

$(base_dir)/src/filter_crcf.o : %.o : %.c $(headers) $(filter_includes)

$(base_dir)/src/filter_cccf.o : %.o : %.c $(headers) $(filter_includes)

$(base_dir)/src/firdes.o : %.o : %.c $(headers)

$(base_dir)/src/firdespm.o : %.o : %.c $(headers)

$(base_dir)/src/group_delay.o : %.o : %.c $(headers)

$(base_dir)/src/hM3.o : %.o : %.c $(headers)

$(base_dir)/src/iirdes.pll.o : %.o : %.c $(headers)

$(base_dir)/src/iirdes.o : %.o : %.c $(headers)

$(base_dir)/src/lpc.o : %.o : %.c $(headers)

$(base_dir)/src/rcos.o : %.o : %.c $(headers)

$(base_dir)/src/rkaiser.o : %.o : %.c $(headers)

$(base_dir)/src/rrcos.o : %.o : %.c $(headers)

$(base_dir)/src/nyquist.o : %.o : %.c $(headers)


# local_tests
#
# This is a list of local autotest scripts (header files) which
# are used to generate the autotest program with the 'check'
# target.  These files are located under the tests/ subdirectory
local_autotests :=					\
	$(base_dir)/tests/decim_autotest.h		\
	$(base_dir)/tests/firdes_autotest.h		\
	$(base_dir)/tests/firdespm_autotest.h		\
	$(base_dir)/tests/firfilt_rrrf_autotest.h	\
	$(base_dir)/tests/firfilt_crcf_autotest.h	\
	$(base_dir)/tests/firhilb_autotest.h		\
	$(base_dir)/tests/firpfb_autotest.h		\
	$(base_dir)/tests/groupdelay_autotest.h		\
	$(base_dir)/tests/iirdes_autotest.h		\
	$(base_dir)/tests/iirfilt_rrrf_autotest.h	\
	$(base_dir)/tests/iirfilt_crcf_autotest.h	\
	$(base_dir)/tests/iirfiltsos_rrrf_autotest.h	\
	$(base_dir)/tests/interp_autotest.h		\



# local_benchmarks
#
# This is a list of local benchmark scripts which are used to
# generate the benchmark program with the 'bench' target.
# These files are located under the bench/ subdirectory
local_benchmarks :=					\
	$(base_dir)/bench/decim_benchmark.h		\
	$(base_dir)/bench/firhilb_benchmark.h		\
	$(base_dir)/bench/firfilt_rrrf_benchmark.h	\
	$(base_dir)/bench/iirfilt_benchmark.h		\
	$(base_dir)/bench/interp_benchmark.h		\
	$(base_dir)/bench/resamp_benchmark.h		\
	$(base_dir)/bench/resamp2_benchmark.h		\
	$(base_dir)/bench/symsync_benchmark.h		\


# Build the local library and local object files
local_library	:= lib$(module_name).a
$(local_library): $(local_objects)
	$(AR) $(ARFLAGS) $@ $^

# accumulate targets
objects			+= $(local_objects)
libraries		+= $(local_library)
autotest_headers	+= $(local_autotests)
benchmark_headers	+= $(local_benchmarks)

