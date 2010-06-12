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
# Makefile for buffer module
#
module_name     := buffer

# local_s_files
#
# This is a list of local source files to compile into objects,
# referenced from the src/ subdirectory under $(local_dir)
#
local_s_files	:= 	\
	bufferf.c	\
	buffercf.c	\
	gport.c
#	bufferui.c

# local_t_files
#
# This is a list of local autotest scripts (header files) which
# are used to generate the autotest program with the 'check'
# target.  These files are located under the tests/ subdirectory
# within $(local_dir)
#
local_t_files	:= 		\
	cbuffer_autotest.h	\
	sbuffer_autotest.h	\
	gport_autotest.h	\
	window_autotest.h


# local_b_files
#
# This is a list of local benchmark scripts which are used to
# generate the benchmark program with the 'bench' target.
# These files are located under the bench/ subdirectory within
# $(local_dir)
#
local_b_files	:= 			\
	gport_dma_benchmark.h		\
	gport_dma_threaded_benchmark.h	\
	gport_ima_benchmark.h		\
	gport_ima_threaded_benchmark.h	\
	window_benchmark.h

include common.mk

