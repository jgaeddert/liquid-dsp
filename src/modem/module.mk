# 
# Makefile for modem module
#


local_dir	:= src/modem
local_srcdir	:= src
local_testdir	:= tests
local_lib	:= libmodem.a
xblah		:= modem.c modem_create.c modem_common.c modulate.c demodulate.c
local_headers	:= modem.h
local_tests	:= modem_autotest.h

local_src	:= $(addprefix $(local_dir)/$(local_srcdir)/, $(xblah))
local_objs	:= $(subst .c,.o,$(local_src))

# accumulate
libraries	+= $(local_lib)
sources		+= $(local_src)
#examples	+= $(local_src)
autotests	+= $(addprefix $(local_dir)/$(local_testdir)/, $(local_tests))
#benchmarks	+= $(local_src)

#$(local_objs): $(local_src)
#	gcc -c $< -o $@

$(local_lib): $(local_objs)
	ar r $@ $^
