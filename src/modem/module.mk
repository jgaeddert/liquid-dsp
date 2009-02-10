# 
# Makefile for modem module
#

# local_dir: local library directory
# 
local_dir	:= src/modem
local_lib	:= libmodem.a

# local_s_files
#
# This is a list of local source files to compile into objects,
# referenced from the src/ subdirectory under $(local_dir)
#
local_s_files	:= modem.c modem_create.c modem_common.c modulate.c demodulate.c

# local_h_files
#
# This is a list of local header files upon which the objects
# depend.  These files are also located under the src/
# subdirectory within $(local_dir)
#
local_h_files	:= modem_internal.h

# local_t_files
#
# This is a list of local autotest scripts (header files) which
# are used to generate the autotest program with the 'check'
# target.  These files are located under the tests/ subdirectory
# within $(local_dir)
#
local_t_files	:= modem_autotest.h


# local_b_files
#
# This is a list of local benchmark scripts which are used to
# generate the benchmark program with the 'bench' target.
# These files are located under the bench/ subdirectory within
# $(local_dir)
#
local_b_files	:= modem_modulate_benchmark.h modem_demodulate_benchmark.h


# local_e_files
#
# This is a list of local example files to demonstrate
# functionality of the module.
#
local_e_files	:= modem_example.c


# 
#
#
local_src	:= $(addprefix $(local_dir)/src/, $(local_s_files))
local_objs	:= $(subst .c,.o,$(local_src))
local_bench	:= $(addprefix $(local_dir)/bench/, $(local_b_files))
local_tests	:= $(addprefix $(local_dir)/tests/, $(local_t_files))
local_examples	:= $(addprefix $(local_dir)/examples/, $(local_e_files))

# accumulated targets
#
# This is a list of target accumulated by the project-level
# makefile.
#
libraries	+= $(local_lib)
sources		+= $(local_src)
autotests	+= $(local_tests)
benchmarks	+= $(local_bench)
examples	+= $(local_examples)

# Build the local library and local object files
$(local_lib): $(local_objs)
	$(AR) $(ARFLAGS) $@ $^

