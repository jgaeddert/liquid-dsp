# 
# Makefile for utility module
#


local_dir	:= src/utility
local_lib	:= libutility.a
blah		:= src/count_bits.c src/pack_bytes.c src/msb_index.c
local_src	:= $(addprefix $(local_dir)/, $(blah))
#local_src	:= src/utility/src/count_bits.c
#	src/count_bits.c 	\
#	src/pack_bytes.c 	\
#	src/msb_index.c
local_objs	:= $(subst .c,.o,$(local_src))

libraries	+= $(local_lib)
sources		+= $(local_src)

#include ../../common.mk

$(local_lib): $(local_objs)
	ar r $@ $^
