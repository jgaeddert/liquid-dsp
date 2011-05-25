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
# Makefile for experimental module
#
module_name	:= experimental
base_dir	:= src/$(module_name)

# local object files
# 
# This is a list of local object files; dependencies are
# described below
local_objects :=				\
	$(base_dir)/src/ann.nodes.o		\
	$(base_dir)/src/activation_functions.o	\
	$(base_dir)/src/cpmodem.o		\
	$(base_dir)/src/kmeans.o		\
	$(base_dir)/src/layer.o			\
	$(base_dir)/src/maxnet.o		\
	$(base_dir)/src/node.o			\
	$(base_dir)/src/fbasc.o			\
	$(base_dir)/src/gport.o			\
	$(base_dir)/src/filter_rrrf.o		\
	$(base_dir)/src/filter_crcf.o		\
	$(base_dir)/src/filter_cccf.o		\
	$(base_dir)/src/gmskframegen.o		\
	$(base_dir)/src/gmskframesync.o		\
	$(base_dir)/src/ofdmframe64.common.o	\
	$(base_dir)/src/ofdmframe64gen.o	\
	$(base_dir)/src/ofdmframe64sync.o	\
	$(base_dir)/src/patternset.o		\
	$(base_dir)/src/awgn_channel.o		\
	$(base_dir)/src/lognorm_channel.o	\
	$(base_dir)/src/channel.o		\
	$(base_dir)/src/pamodel.o		\
	$(base_dir)/src/ricek_channel.o		\


# list explicit targets and dependencies here

filter_includes :=			\
	$(base_dir)/src/iirqmfb.c	\
	$(base_dir)/src/itqmfb.c	\
	$(base_dir)/src/qmfb.c		\
	$(base_dir)/src/symsync2.c	\
	$(base_dir)/src/symsynclp.c	\

$(base_dir)/src/filter_rrrf.o: %.o : %.c $(headers) $(filter_includes)

$(base_dir)/src/filter_crcf.o: %.o : %.c $(headers) $(filter_includes)

$(base_dir)/src/filter_cccf.o: %.o : %.c $(headers) $(filter_includes)



# local_tests
#
# This is a list of local autotest scripts (header files) which
# are used to generate the autotest program with the 'check'
# target.  These files are located under the tests/ subdirectory
local_autotests :=					\
	$(base_dir)/tests/ann_autotest.h		\
	$(base_dir)/tests/gport_autotest.h		\
	$(base_dir)/tests/channel_autotest.h		\


# local_benchmarks
#
# This is a list of local benchmark scripts which are used to
# generate the benchmark program with the 'bench' target.
# These files are located under the bench/ subdirectory
local_benchmarks :=					\
	$(base_dir)/bench/fbasc_benchmark.h		\
	$(base_dir)/bench/gport_dma_benchmark.h		\
	$(base_dir)/bench/gport_dma_threaded_benchmark.h	\
	$(base_dir)/bench/gport_ima_benchmark.h		\
	$(base_dir)/bench/gport_ima_threaded_benchmark.h	\
	$(base_dir)/bench/dds_benchmark.h		\
	$(base_dir)/bench/ofdmframe64sync_benchmark.h		\
	$(base_dir)/bench/qmfb_benchmark.h		\


# Build the local library and local object files
local_library	:= lib$(module_name).a
$(local_library): $(local_objects)
	$(AR) $(ARFLAGS) $@ $^

# accumulate targets
objects			+= $(local_objects)
libraries		+= $(local_library)
autotest_headers	+= $(local_autotests)
benchmark_headers	+= $(local_benchmarks)

