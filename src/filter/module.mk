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
# Makefile for filter module
#
module_name     := filter

# local_s_files
#
# This is a list of local source files to compile into objects,
# referenced from the src/ subdirectory under $(local_dir)
#
local_s_files	:= 			\
	filter_rrrf.c			\
	filter_crcf.c			\
	filter_cccf.c			\
	filter_common.c			\
	firdes.c			\
	rcos.c				\
	rrcos.c				\
	lf2.c
#	nyquist.c

# local_h_files
#
# This is a list of local header files upon which the objects
# depend.  These files are also located under the src/
# subdirectory within $(local_dir)
#
local_h_files	:=

# local_t_files
#
# This is a list of local autotest scripts (header files) which
# are used to generate the autotest program with the 'check'
# target.  These files are located under the tests/ subdirectory
# within $(local_dir)
#
local_t_files	:= 			\
	decim_autotest.h		\
	firdes_autotest.h		\
	fir_filter_rrrf_autotest.h	\
	fir_filter_crcf_autotest.h	\
	firhilb_autotest.h		\
	firpfb_autotest.h		\
	iir_filter_rrrf_autotest.h	\
	iir_filter_crcf_autotest.h	\
	interp_autotest.h


# local_b_files
#
# This is a list of local benchmark scripts which are used to
# generate the benchmark program with the 'bench' target.
# These files are located under the bench/ subdirectory within
# $(local_dir)
#
local_b_files	:=			\
	decim_benchmark.h		\
	firhilb_benchmark.h		\
	fir_filter_benchmark.h		\
	iir_filter_benchmark.h		\
	interp_benchmark.h		\
	qmfb_benchmark.h		\
	resamp2_benchmark.h		\
	symsync_benchmark.h


include common.mk

