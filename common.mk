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

