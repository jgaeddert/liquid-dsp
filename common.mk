# Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
# Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
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
# Common module makefile
#
# This makefile is included by each module's "module.mk" and expands
# the variables specific to each module.  This is done for compactness,
# however it might be a bit confusing.
#
# Each module defines these local variables:
#
#    module_name    : name of the module (e.g. filter)
#    local_s_files  : list of local source files (.c)
#    local_t_files  : list of local autotest headers
#    local_b_files  : list of local benchmark headers
#
# This makefile expands these variables to their names relative to
# the top srcdir directory.  For example, say the module 'filter'
# defines $(local_s_files) as
#
#     local_s_files := filter_crcf.c \
#                      filter_cccf.c \
#                      filter_rrrf.c
#
# then this common.mk expands $(local_src) as
#
#     local_src := src/filter/src/filter_crcf.c \
#                  src/filter/src/filter_cccf.c \
#                  src/filter/src/filter_rrrf.c
#
# and appends it to the top-level variable $(sources) variable. This
# trend continues across the tests and benchmarks files.

local_dir       := $(modulesdir)/$(module_name)
local_lib       := lib$(module_name).a

local_src	:= $(addprefix $(local_dir)/$(modulesdir)/, $(local_s_files))
local_objs	:= $(patsubst %.c,%.o,$(local_src))
local_bench	:= $(addprefix $(local_dir)/bench/, $(local_b_files))
local_tests	:= $(addprefix $(local_dir)/tests/, $(local_t_files))

# accumulated targets
#
# This is a list of target accumulated by the project-level
# makefile.
#
libraries		+= $(local_lib)
sources			+= $(local_src)
autotest_headers	+= $(local_tests)
benchmark_headers	+= $(local_bench)

# Build the local library and local object files
$(local_lib): $(local_objs)
	$(AR) $(ARFLAGS) $@ $^

