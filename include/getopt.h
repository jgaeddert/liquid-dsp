/*
 * Windows compatibility shim for getopt.h
 * Provides getopt declarations for MSVC builds
 *
 * Copyright (c) 2007 - 2025 Joseph Gaeddert
 * MIT License
 */
#ifndef __LIQUID_GETOPT_H__
#define __LIQUID_GETOPT_H__

#ifdef _MSC_VER
/* MSVC doesn't have getopt.h, use our compatibility layer */
#include "liquid_win32_compat.h"
#else
/* MinGW and other platforms have native getopt.h */
#include_next <getopt.h>
#endif

#endif /* __LIQUID_GETOPT_H__ */
