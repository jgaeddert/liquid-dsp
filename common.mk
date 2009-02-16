# 
# Common makefile
#

local_src	:= $(addprefix $(local_dir)/src/, $(local_s_files))
local_objs	:= $(patsubst %.c,%.o,$(local_src))
local_bench	:= $(addprefix $(local_dir)/bench/, $(local_b_files))
local_tests	:= $(addprefix $(local_dir)/tests/, $(local_t_files))
#local_examples	:= $(addprefix examples/, $(local_e_files))

# accumulated targets
#
# This is a list of target accumulated by the project-level
# makefile.
#
libraries	+= $(local_lib)
sources		+= $(local_src)
autotests	+= $(local_tests)
benchmarks	+= $(local_bench)
#examples	+= $(local_examples)

# Build the local library and local object files
$(local_lib): $(local_objs)
	$(AR) $(ARFLAGS) $@ $^

