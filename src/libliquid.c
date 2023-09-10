/*
 * Copyright (c) 2007 - 2023 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// Run-time library version numbers, error handling, logging

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "liquid.internal.h"

#define LIQUID_LOG_TIMESTAMP        (1U << 9)   // log the timestamp

#define LIQUID_LOG_LEVEL_FULL       (1U << 8)   // log the full level, e.g. "warning", "info"
#define LIQUID_LOG_LEVEL_4          (1U << 7)   // log the level truncated to 4 characters, e.g. "warn", "info"
#define LIQUID_LOG_LEVEL_1          (1U << 6)   // log the level truncated to a single character, e.g. "W", "I"

#define LIQUID_LOG_FILENAME_FULL    (1U << 5)   // log the full filename
#define LIQUID_LOG_FILENAME_32      (1U << 4)   // log the filename, trucated to 32 characters maximum
#define LIQUID_LOG_FILENAME_20      (1U << 3)   // log the filename, trucated to 20 characters maximum
#define LIQUID_LOG_FILENAME_12      (1U << 2)   // log the filename, trucated to 12 characters maximum

#define LIQUID_LOG_LINE             (1U << 1)   // log the line number

#define LIQUID_LOG_COLOR            (1U << 0)   // log in color

#define LIQUID_LOG_LEVEL            LIQUID_LOG_LEVEL_FULL
#define LIQUID_LOG_FILENAME         LIQUID_LOG_FILENAME_20

// some default settings
#define LIQUID_LOG_FULL    (LIQUID_LOG_TIMESTAMP | LIQUID_LOG_LEVEL_FULL | LIQUID_LOG_FILENAME_FULL | LIQUID_LOG_LINE )
#define LIQUID_LOG_CONCISE (LIQUID_LOG_TIMESTAMP | LIQUID_LOG_LEVEL_4    | LIQUID_LOG_FILENAME_20   | LIQUID_LOG_LINE )
#define LIQUID_LOG_COMPACT (LIQUID_LOG_TIMESTAMP | LIQUID_LOG_LEVEL_1    | LIQUID_LOG_FILENAME_12   | LIQUID_LOG_LINE )
#define LIQUID_LOG_DEFAULT LIQUID_LOG_FULL

// internal flag indicating if errors are downgraded to warnings for testing
static int _liquid_error_downgrade = 0;

// enable downgrade errors to warnings
void _liquid_error_downgrade_enable (void) { _liquid_error_downgrade = 1; }

// disable downgrade errors to warnings
void _liquid_error_downgrade_disable(void) { _liquid_error_downgrade = 0; }

// forward declaration of logging function with variadic arguments list
int liquid_vlog(liquid_logger _q,
                int           _level,
                const char *  _file,
                int           _line,
                const char *  _format,
                va_list       _ap);

const char liquid_version[] = LIQUID_VERSION;

const char * liquid_libversion(void)
{
    return LIQUID_VERSION;
}

int liquid_libversion_number(void)
{
    return LIQUID_VERSION_NUMBER;
}

const char * liquid_log_colors[] = {"\033[94m","\033[36m","\033[32m","\033[33m","\033[31m","\033[35m"};

const char * liquid_log_levels[] = {"trace","debug","info","warning","error","fatal"};

// report error
int liquid_error_fl(int          _code,
                    const char * _file,
                    int          _line,
                    const char * _format,
                    ...)
{
#if !LIQUID_SUPPRESS_ERROR_OUTPUT
    // generate extended format
    int  format_len = strlen(_format) + 20 + strlen(liquid_error_info(_code));
    char format_ext[format_len];
    snprintf(format_ext,format_len,"%s (code %u: %s)",
        _format, _code, liquid_error_info(_code));

    // log error
    va_list argptr;
    va_start(argptr, _format);
    liquid_vlog(NULL,_liquid_error_downgrade ? LIQUID_WARN : LIQUID_ERROR,_file,_line,format_ext,argptr);
    va_end(argptr);
#endif
#if LIQUID_STRICT_EXIT
    if (!_liquid_error_downgrade)
        exit(_code);
#endif
    return _code;
}

// report error
void * liquid_error_config_fl(const char * _file,
                              int          _line,
                              const char * _format,
                              ...)
{
    // TODO: call variadic arguments version of liquid_error_fl
    int code = LIQUID_EICONFIG;
#if !LIQUID_SUPPRESS_ERROR_OUTPUT
    // generate extended format
    int  format_len = strlen(_format) + 20 + strlen(liquid_error_info(code));
    char format_ext[format_len];
    snprintf(format_ext,format_len,"%s (code %u: %s)",
        _format, code, liquid_error_info(code));

    // log error
    va_list argptr;
    va_start(argptr, _format);
    liquid_vlog(NULL,_liquid_error_downgrade ? LIQUID_WARN : LIQUID_ERROR,_file,_line,format_ext,argptr);
    va_end(argptr);
#endif
#if LIQUID_STRICT_EXIT
    if (!_liquid_error_downgrade)
        exit(code);
#endif
    return NULL;
}

const char * liquid_error_str[LIQUID_NUM_ERRORS] = {
    "ok",                                                   // LIQUID_OK
    "internal logic error",                                 // LIQUID_EINT
    "invalid object",                                       // LIQUID_EIOBJ,
    "invalid parameter or configuration",                   // LIQUID_EICONFIG
    "input out of range",                                   // LIQUID_EIVAL
    "invalid vector length or dimension",                   // LIQUID_EIRANGE
    "invalid mode",                                         // LIQUID_EIMODE
    "unsupported mode",                                     // LIQUID_EUMODE
    "object has not been created or properly initialized",  // LIQUID_ENOINIT
    "not enough memory allocated for operation",            // LIQUID_EIMEM
    "file input/output",                                    // LIQUID_EIO
    "algorithm could not converge",                         // LIQUID_ENOCONV
    "function or method not yet implemented",               // LIQUID_ENOIMP
};

// get error string given code
const char * liquid_error_info(liquid_error_code _code)
{
    if (_code < 0 || _code >= LIQUID_NUM_ERRORS) {
        liquid_error(LIQUID_EIMODE,"error code %d is out of range", _code);
        return NULL;
    }

    return liquid_error_str[_code];
}

#define LIQUID_LOGGER_MAX_CALLBACKS (32)
struct liquid_logger_s {
    int     level;
    //int timezone;
    char time_fmt[16];  // format of time to pass to strftime, default:"%T"
    int  config;        // display configuration
    liquid_log_callback cb_function[LIQUID_LOGGER_MAX_CALLBACKS]; // callback function
    void *              cb_context [LIQUID_LOGGER_MAX_CALLBACKS]; // callback context
    int                 cb_level   [LIQUID_LOGGER_MAX_CALLBACKS]; // callback level
    int count[6];       // counters showing number of events of each type
    liquid_lock_callback lock_callback; // locking callback function
    void *               lock_context;  // locking context
};


// global logger
static struct liquid_logger_s qlog = {
    .level         = 0,
    .time_fmt      = "[%T] ",
    .config        = (LIQUID_LOG_DEFAULT | LIQUID_LOG_COLOR),
    .cb_function   = {NULL,},
    .count         = {0,0,0,0,0,0,},
    .lock_callback = NULL,
    .lock_context  = NULL,
};

// internal methods
liquid_logger liquid_logger_safe_cast(liquid_logger _q)
    { return _q == NULL ? &qlog : _q; }

// log to stdout/stderr
int liquid_logger_callback_stream(liquid_log_event _event,
                                  FILE * restrict  _stream,
                                  int              _config)
{
    if (ftell(_stream) < 0)
        return 0; // file/stream is not open

    // compactness levels:
    // <=0:  [09:17:53] warning: src/multichannel/src/firpfbch2.proto.c:183: firfilt_crcf_copy(), object cannot be NULL (code 3: invalid parameter or configuration)
    //   1:  [09:17:53] warn:…c/firpfbch2.proto.c:183:firfilt_crcf_copy(), object cannot be NULL (code 3: invalid parameter or configuration)
    //   2:  W:…bch2.proto.c:183:firfilt_crcf_copy(), object cannot be NULL (code 3: invalid parameter or configuration)
    // >=3:  W:firfilt_crcf_copy(), object cannot be NULL (code 3: invalid parameter or configuration)
#if 0
    // compact
    // TODO: look for path delimiters using strtok?
    int smax = 20;
    fprintf(_stream,"%s%c\033[0m:",
        liquid_log_colors[_event->level],
        liquid_log_levels[_event->level][0]-32);
    int slen = strlen(_event->file);
    if (slen  > smax) {
        fprintf(_stream,"\033[90m\u2026%*s:%-3d:\033[0m",
        smax - 1,
        _event->file + slen - smax + 1,
        _event->line);
    } else {
        fprintf(_stream,"\033[90m%*s:%-3d:\033[0m",
        smax,
        _event->file,
        _event->line);
    }
#else
    // parse configuration
    unsigned int enable_color = _config & LIQUID_LOG_COLOR;

    // print timestamp
    if (_config & LIQUID_LOG_TIMESTAMP)
        fprintf(_stream,"%s",_event->time_str);

    // print log level
    if (enable_color) {
        fprintf(_stream,"%s%s\033[0m: ",
            liquid_log_colors[_event->level],
            liquid_log_levels[_event->level]);
    } else {
        fprintf(_stream,"%s: ",liquid_log_levels[_event->level]);
    }

    // print file/line
    if (enable_color) {
        fprintf(_stream,"\033[90m%s:%d:\033[0m ",_event->file,_event->line);
    } else {
        fprintf(_stream,"%s:%d: ",_event->file,_event->line);
    }
#endif

    // parse variadic function arguments
    vfprintf(_stream, _event->format, _event->args);
    fprintf(_stream,"\n");
    return LIQUID_OK;
}

// log to file
int liquid_logger_callback_file(liquid_log_event _event,
                                void *           _fid)
{
    return liquid_logger_callback_stream(_event, (FILE*)_fid, LIQUID_LOG_DEFAULT);
}


liquid_logger liquid_logger_create()
{
    liquid_logger q = (liquid_logger) malloc(sizeof(struct liquid_logger_s));
    liquid_logger_reset(q);
    return q;
}

int liquid_logger_destroy(liquid_logger _q)
{
    free(_q);
    return LIQUID_OK;
}

int liquid_logger_reset(liquid_logger _q)
{
    _q = liquid_logger_safe_cast(_q);
    _q->level = LIQUID_WARN;
    liquid_logger_set_time_fmt(_q, "[%T]");
    _q->cb_function[0] = NULL; // effectively reset all callbacks
    int i;
    for (i=0; i<6; i++)
        _q->count[i] = 0;
    return LIQUID_OK;
}

int liquid_logger_print(liquid_logger _q)
{
    _q = liquid_logger_safe_cast(_q);
    printf("<liquid_logger, level:%s, callbacks:%u, fmt:%s, count:",
        // TODO: validate
        liquid_log_levels[_q->level],
        liquid_logger_get_num_callbacks(_q),
        _q->time_fmt);

    // print event counts
    printf("(");
    int i;
    for (i=0; i<6; i++)
        printf("%u,", _q->count[i]);
    printf(")>\n");
    return LIQUID_OK;
}

int liquid_logger_set_level(liquid_logger _q,
                            int           _level)
{
    _q = liquid_logger_safe_cast(_q);
    _q->level = _level;
    return LIQUID_OK;
}

int liquid_logger_set_time_fmt(liquid_logger _q,
                               const char * _fmt)
{
    _q = liquid_logger_safe_cast(_q);

    // handle special case if input is empty
    if (_fmt == NULL || strlen(_fmt)==0) {
        _q->time_fmt[0] = '\0';
        return LIQUID_OK;
    }

    // set format and validate copy operation was successful
    int rc = snprintf(_q->time_fmt, sizeof(_q->time_fmt), "%s ", _fmt);
    if (rc >= sizeof(_q->time_fmt)) {
        _q->time_fmt[0] = '\0';
        return liquid_error(LIQUID_EIMEM,"liquid_logger_set_time_fmt(), format \"%s\" length exceeds maximum", _fmt);
    }
    return LIQUID_OK;
}

// add lock function with context
//  _q          : logger object
//  _callback   : user-defined lock callback function
//  _context    : context passed when callback is invoked
int liquid_logger_set_lock(liquid_logger        _q,
                           liquid_lock_callback _callback,
                           void *               _context)
{
    _q->lock_callback = _callback;
    _q->lock_context  = _context;
    return LIQUID_OK;
}

int liquid_logger_add_callback(liquid_logger       _q,
                               liquid_log_callback _callback,
                               void *              _context,
                               int                 _level)
{
    _q = liquid_logger_safe_cast(_q);
    unsigned int index = liquid_logger_get_num_callbacks(_q);

    // check that maximum has not been reached already
    if (index==LIQUID_LOGGER_MAX_CALLBACKS)
        return liquid_error(LIQUID_EICONFIG,"maximum number of callbacks (%u) reached", LIQUID_LOGGER_MAX_CALLBACKS);

    // set callback, context, and level at this index
    _q->cb_function [index] = _callback;
    _q->cb_context[index] = _context;
    _q->cb_level  [index] = _level;

    // assign next position NULL pointer, assuming not already full
    if (index < LIQUID_LOGGER_MAX_CALLBACKS-1)
        _q->cb_function[index+1] = NULL;

    return LIQUID_OK;
}

int liquid_logger_add_file(liquid_logger _q,
                           FILE *        _fid,
                           int           _level)
{
    _q = liquid_logger_safe_cast(_q);
    return liquid_logger_add_callback(_q, liquid_logger_callback_file, (void*)_fid, _level);
}

unsigned int liquid_logger_get_num_callbacks(liquid_logger _q)
{
    _q = liquid_logger_safe_cast(_q);
    // first get index of NULL
    unsigned int i;
    for (i=0; i<LIQUID_LOGGER_MAX_CALLBACKS; i++) {
        if (_q->cb_function[i] == NULL)
            break;
    }
    return i;
}

int liquid_log(liquid_logger _q,
               int           _level,
               const char *  _file,
               int           _line,
               const char *  _format,
               ...)
{
    va_list ap;
    va_start(ap, _format);
    int rv = liquid_vlog(_q, _level, _file, _line, _format, ap);
    va_end(ap);
    return rv;
}

int liquid_vlog(liquid_logger _q,
                int           _level,
                const char *  _file,
                int           _line,
                const char *  _format,
                va_list       _ap)
{
    // set to global object if input is NULL (default)
    _q = liquid_logger_safe_cast(_q);

    // validate level
    if (_level < 0 || _level >= 6)
        return liquid_error(LIQUID_EIRANGE,"log level (%d) out of range", _level);

    // update count
    _q->count[_level]++;

    // create event
    time_t t = time(NULL);
    struct liquid_log_event_s event = {
        .format    = _format,
        .file      = _file,
        .line      = _line,
        .level     = _level,
        .timestamp = localtime(&t),
    };
    event.time_str[
        strftime(event.time_str, sizeof(event.time_str), _q->time_fmt, event.timestamp)
    ] = '\0';

    // lock
    if (_q->lock_callback != NULL)
        _q->lock_callback(1, _q->lock_context);

    // output to stdout
    if (_level >= _q->level) {
        va_copy(event.args, _ap);
        liquid_logger_callback_stream(&event, stderr, _q->config);
        va_end(event.args);
    }

    // invoke callbacks
    int i;
    for (i=0; i<LIQUID_LOGGER_MAX_CALLBACKS && _q->cb_function[i] != NULL; i++) {
        if (_level >= _q->cb_level[i]) {
            va_copy(event.args, _ap);
            _q->cb_function[i](&event, _q->cb_context[i]);
            va_end(event.args);
        }
    }

    // unlock
    if (_q->lock_callback != NULL)
        _q->lock_callback(0, _q->lock_context);

    return LIQUID_OK;
}

