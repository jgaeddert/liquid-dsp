# 
# Examples
#

local_dir	:= examples
local_src	:=			\
	agc_example.c			\
	buffer_example.c		\
	cfbuffer_example.c		\
	dotprod_rrrf_example.c		\
	dotprod_cccf_example.c		\
	fbuffer_example.c		\
	fwindow_example.c		\
	fading_channel_example.c	\
	fading_generator_example.c	\
	gport2_example.c		\
	gport_example.c			\
	gport_threaded_example.c	\
	gport2_threaded_example.c	\
	modem_example.c			\
	ricek_channel_example.c
#	metadata_example.c		\

local_examples	:= $(addprefix $(local_dir)/, $(local_src))

examples	= $(local_examples)

