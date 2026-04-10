/*
 * Windows compatibility shim for unistd.h
 * Provides minimal POSIX-like functionality for Windows
 *
 * Copyright (c) 2007 - 2025 Joseph Gaeddert
 * MIT License
 */
#ifndef __LIQUID_UNISTD_H__
#define __LIQUID_UNISTD_H__

#ifdef _WIN32

#include <io.h>
#include <process.h>

/* Map POSIX functions to Windows equivalents */
#define usleep(us) Sleep((us) / 1000)

/* For Windows, include windows.h for Sleep() */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#else
/* On non-Windows, include the real system unistd.h */
#include_next <unistd.h>
#endif

#endif /* __LIQUID_UNISTD_H__ */
