# 
# Examples
#

local_dir	:= examples
local_src	:=		\
	agc_example.c		\
	modem_example.c

local_examples	:= $(addprefix $(local_dir)/, $(local_src))

examples	= $(local_examples)

