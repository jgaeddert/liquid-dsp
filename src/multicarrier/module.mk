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
# Makefile for multicarrier module
#
module_name     := multicarrier

# local_s_files
#
# This is a list of local source files to compile into objects,
# referenced from the src/ subdirectory under $(local_dir)
#
local_s_files	:=			\
	firpfbch_crcf.c			\
	firpfbch_cccf.c			\
	ofdmframe64.common.c		\
	ofdmframe64gen.c		\
	ofdmframe64sync.c		\
	ofdmframe.common.c		\
	ofdmframegen.c			\
	ofdmframesync.c			\
	ofdmoqam.c			\
	ofdmoqamframe.common.c		\
	ofdmoqamframegen.c		\
	ofdmoqamframesync.c

# local_t_files
#
# This is a list of local autotest scripts (header files) which
# are used to generate the autotest program with the 'check'
# target.  These files are located under the tests/ subdirectory
# within $(local_dir)
#
local_t_files	:=			\
	firpfbch_synthesizer_autotest.h	\
	firpfbch_analyzer_autotest.h	\
	ofdmoqam_autotest.h


# local_b_files
#
# This is a list of local benchmark scripts which are used to
# generate the benchmark program with the 'bench' target.
# These files are located under the bench/ subdirectory within
# $(local_dir)
#
local_b_files	:=			\
	firpfbch_benchmark.h		\
	ofdmframe64sync_benchmark.h	\
	ofdmframesync_acquire_benchmark.h	\
	ofdmframesync_rxsymbol_benchmark.h	\
	ofdmoqam_benchmark.h


include common.mk

