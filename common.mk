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
# Common module makefile
#

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

