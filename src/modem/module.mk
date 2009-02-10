# 
# Makefile for modem module
#


local_dir	:= src/modem
local_lib	:= libmodem.a
xblah		:= src/modem.c src/modem_create.c src/modem_common.c src/modulate.c src/demodulate.c
local_headers	:= src/modem/src/modem.h
local_src	:= $(addprefix $(local_dir)/, $(xblah))
local_objs	:= $(subst .c,.o,$(local_src))

# accumulate
libraries	+= $(local_lib)
sources		+= $(local_src)
#examples	+= $(local_src)
#autotests	+= $(local_src)
#benchmarks	+= $(local_src)

#$(local_objs): $(local_src)
#	gcc -c $< -o $@

$(local_lib): $(local_objs)
	ar r $@ $^
