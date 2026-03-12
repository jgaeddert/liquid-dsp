/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
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

// Run-time logging

#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "liquid.internal.h"

#ifdef LIQUID_COLOR_ENABLE
    // enable ANSI escape colors
    const char * liquid_log_color_soft  = "\033[90m";
    const char * liquid_log_color_clear = "\033[0m";
    const char * liquid_log_colors[LIQUID_LOG_NUM_LEVELS] =
        {"\033[94m","\033[36m","\033[32m","\033[33m","\033[31m","\033[35m"};
#else
    // disable ANSI escape colors
    const char * liquid_log_color_soft  = "";
    const char * liquid_log_color_clear = "";
    const char * liquid_log_colors[LIQUID_LOG_NUM_LEVELS] = {"","","","","","",};
#endif

const char * liquid_log_levels[LIQUID_LOG_NUM_LEVELS] =
    {"trace","debug","info", "warning","error","fatal"};

const char * liquid_log_levels_concise[LIQUID_LOG_NUM_LEVELS] =
    {"trace","debug","info ","warn ",  "error","fatal"};

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

#ifdef LIQUID_LOGGING_ENABLE

// global logger
static struct liquid_logger_s qlog = {
    .level         = 0,
    .time_fmt      = "%Y-%m-%d %T",
    .config        = (LIQUID_LOG_DEFAULT | LIQUID_LOG_COLOR),
    .cb_function   = {NULL,},
    .count         = {0,0,0,0,0,0,},
    .lock_callback = NULL,
    .lock_context  = NULL,
};

// internal methods
liquid_logger liquid_logger_safe_cast(liquid_logger _q)
    { return _q == NULL ? &qlog : _q; }

// log filename and line number to stream output
int liquid_logger_stream_file_line(liquid_log_event _event,
                                   FILE * restrict  _stream,
                                   int              _color,
                                   int              _smax,
                                   int              _line)
{
    if (_color)
        fprintf(_stream,"%s",liquid_log_color_soft);

    if (_smax < 0) {
        // print full file/line
        fprintf(_stream,"%s:",_event->file);
    } else if (_smax > 0) {
        // print compact file/line
        // TODO: look for path delimiters using strtok?
        int slen = strlen(_event->file);
        if (slen  > _smax) {
            fprintf(_stream,"\u2026%*s:", _smax - 1, _event->file + slen - _smax + 1);
        } else {
            fprintf(_stream,"%*s:",_smax, _event->file);
        }
    } else {
        // _smax == 0; don't print filename at all
    }

    if (_line)
        fprintf(_stream,"%d:",_event->line);

    if (_color)
        fprintf(_stream,"%s",liquid_log_color_clear);

    if (_smax != 0 || _line)
        fprintf(_stream," ");
    return LIQUID_OK;
}

// log to file stream
int liquid_logger_callback_stream(liquid_log_event _event,
                                  FILE * restrict  _stream,
                                  int              _config)
{
    if (_stream != stdout && _stream != stderr && ftell(_stream) < 0)
        return 0; // file/stream is not open

    // parse configuration
    bool enable_color = _config & LIQUID_LOG_COLOR;

    // print timestamp
    fprintf(_stream,"%s", enable_color ? liquid_log_color_soft : "");
    fprintf(_stream,"%s", _event->time_str);
    fprintf(_stream,"%s", enable_color ? liquid_log_color_clear : "");
    if (strlen(_event->time_str) > 0)
        fprintf(_stream," ");

    if (_config & LIQUID_LOG_LEVEL_BRACKETS)
        fprintf(_stream,"[");

    if (enable_color)
        fprintf(_stream,"%s",liquid_log_colors[_event->level]);

    // print log level string, e.g. "warning", "warn ", "W", or "3"
    if (_config & LIQUID_LOG_LEVEL_FULL) {
        // e.g. "warning"
        fprintf(_stream,"%s",liquid_log_levels[_event->level]);
    } else if (_config & LIQUID_LOG_LEVEL_SHORT) {
        // e.g. "warn "
        fprintf(_stream,"%s", liquid_log_levels_concise[_event->level]);
    } else if (_config & LIQUID_LOG_LEVEL_ONE) {
        // e.g. "W"
        fprintf(_stream,"%c",liquid_log_levels[_event->level][0]-32);
    } else if (_config & LIQUID_LOG_LEVEL_NUMBER) {
        // e.g. "3"
        fprintf(_stream,"%d",_event->level);
    }

    if (enable_color)
        fprintf(_stream,"%s",liquid_log_color_clear);

    if (_config & LIQUID_LOG_LEVEL_BRACKETS)
        fprintf(_stream,"]");
    if (_config & (LIQUID_LOG_LEVEL_FULL | LIQUID_LOG_LEVEL_SHORT |
                   LIQUID_LOG_LEVEL_ONE  |LIQUID_LOG_LEVEL_NUMBER |
                   LIQUID_LOG_LEVEL_BRACKETS))
    {
        fprintf(_stream," ");
    }

    // print file/line
    int smax = 0;
    if      (_config & LIQUID_LOG_FILENAME      ) { smax = -1; }
    else if (_config & LIQUID_LOG_FILENAME_SHORT) { smax = 32; } // TODO: clip filename
    else if (_config & LIQUID_LOG_FILENAME_TRUNC) { smax = 20; }
    liquid_logger_stream_file_line(_event, _stream, enable_color, smax, _config & LIQUID_LOG_LINE);

    // parse variadic function arguments
    vfprintf(_stream, _event->format, _event->args);
    fprintf(_stream,"\n");
    return LIQUID_OK;
}

// log to file
int liquid_logger_callback_file(liquid_log_event _event,
                                void *           _fid,
                                int              _config)
{
    // use same format, but explicitly disable color
    return liquid_logger_callback_stream(_event, (FILE*)_fid, _config & ~LIQUID_LOG_COLOR);
}

// log timestamp and format string
// TODO: pass timespec, string, and config rather than full event
int liquid_event_timestamp(struct liquid_log_event_s * _q, int _config)
{
    // set formatted timestamp
    // NOTE: the string format is hard-coded here
#if 0
    clock_gettime(CLOCK_REALTIME, &(_q->timestamp));
#else
    timespec_get(&(_q->timestamp), TIME_UTC);
#endif
    bool format_utc = _config & LIQUID_LOG_UTC;
    size_t n=0;
    _q->time_str[0] = '\0';
    if (_config & LIQUID_LOG_RAWTIME) {
        n += snprintf(_q->time_str, 64, "%ld", _q->timestamp.tv_sec);
    } else if (_config & LIQUID_LOG_DATETIME) {
        if (format_utc)
            n += strftime(_q->time_str, sizeof(_q->time_str), "%Y-%m-%dT%T", gmtime(&_q->timestamp.tv_sec));
        else
            n += strftime(_q->time_str, sizeof(_q->time_str), "%Y-%m-%d %T", localtime(&_q->timestamp.tv_sec));
    } else if (_config & LIQUID_LOG_DATE) {
        if (format_utc)
            n += strftime(_q->time_str, sizeof(_q->time_str), "%Y-%m-%d", gmtime(&_q->timestamp.tv_sec));
        else
            n += strftime(_q->time_str, sizeof(_q->time_str), "%Y-%m-%d", localtime(&_q->timestamp.tv_sec));
    } else if (_config & LIQUID_LOG_TIME) {
        if (format_utc)
            n += strftime(_q->time_str, sizeof(_q->time_str), "%T", gmtime(&_q->timestamp.tv_sec));
        else
            n += strftime(_q->time_str, sizeof(_q->time_str), "%T", localtime(&_q->timestamp.tv_sec));
    }

    // print fractional seconds if time is used and ms, ns, or us is requested
    bool print_time = _config & (LIQUID_LOG_RAWTIME | LIQUID_LOG_DATETIME | LIQUID_LOG_TIME);

    if (print_time && (_config & LIQUID_LOG_MS))
        sprintf(_q->time_str+n,".%.3ld", _q->timestamp.tv_nsec / 1000000);
    else if (print_time && (_config & LIQUID_LOG_US))
        sprintf(_q->time_str+n,".%.6ld", _q->timestamp.tv_nsec /    1000);
    else if (print_time && (_config & LIQUID_LOG_NS))
        sprintf(_q->time_str+n,".%.9ld", _q->timestamp.tv_nsec          );

    if (format_utc)
        strcat(_q->time_str, "Z");

    return LIQUID_OK;
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

// set output configuration
int liquid_logger_set_config(liquid_logger _q,
                             int           _config)
{
    _q = liquid_logger_safe_cast(_q);
    _q->config = _config;
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
    _q = liquid_logger_safe_cast(_q);
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

FILE * liquid_logger_add_filename(liquid_logger _q,
                                  const char*   _filename,
                                  int           _level)
{
    FILE * fid = fopen(_filename,"w");

    if (fid == NULL)
    {
        liquid_error(LIQUID_EIO,"logger could not open '%s' for writing", _filename);
    } else {
        liquid_logger_add_file(_q, fid, _level);
    }
    return fid;
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
    // validate level
    if (_level < 0 || _level >= 6)
        return liquid_error(LIQUID_EIRANGE,"log level (%d) out of range", _level);

    // set to global object if input is NULL (default)
    _q = liquid_logger_safe_cast(_q);

    // lock
    if (_q->lock_callback != NULL)
        _q->lock_callback(1, _q->lock_context);

    // update count
    _q->count[_level]++;

    // create event
    struct liquid_log_event_s event = {
        .format    = _format,
        .file      = _file,
        .line      = _line,
        .level     = _level,
    };

    // set formatted timestamp
#if 0
    // NOTE: the string format is hard-coded here
    //clock_gettime(CLOCK_REALTIME, &event.timestamp);
    timespec_get(&event.timestamp, TIME_UTC);
    bool format_utc = false;
    bool format_ms  = true;
    size_t n;
    if (format_utc)
        n = strftime(event.time_str, sizeof(event.time_str), "%Y-%m-%dT%T", gmtime(&event.timestamp.tv_sec));
    else
        n = strftime(event.time_str, sizeof(event.time_str), "%Y-%m-%d %T", localtime(&event.timestamp.tv_sec));

    if (format_ms)
        sprintf(event.time_str+n,".%.3ld", event.timestamp.tv_nsec / 1000000);

    if (format_utc)
        strcat(event.time_str, "Z");
#else
    liquid_event_timestamp(&event, _q->config);
#endif

    // output to stdout
    if (_level >= _q->level) {
        va_copy(event.args, _ap);
        liquid_logger_callback_stream(&event, stdout, _q->config);
        va_end(event.args);
    }

    // invoke callbacks
    int i;
    for (i=0; i<LIQUID_LOGGER_MAX_CALLBACKS && _q->cb_function[i] != NULL; i++) {
        if (_level >= _q->cb_level[i]) {
            va_copy(event.args, _ap);
            _q->cb_function[i](&event, _q->cb_context[i], _q->config);
            va_end(event.args);
        }
    }

    // unlock
    if (_q->lock_callback != NULL)
        _q->lock_callback(0, _q->lock_context);

    return LIQUID_OK;
}

int liquid_exit()
{
    return (qlog.count[LIQUID_LOG_NUM_LEVELS-1] + qlog.count[LIQUID_LOG_NUM_LEVELS-2])
        ? -1 : 0;
}

#else // LIQUID_LOGGING_ENABLE

liquid_logger liquid_logger_create()
{
    liquid_error(LIQUID_EICONFIG,"compile-time logging disabled");
    return NULL;
}

// destroy logger object, freeing all internal memory
int liquid_logger_destroy(liquid_logger _q)
    { return liquid_error(LIQUID_EICONFIG,"compile-time logging disabled"); }

// reset internal logger object counters, reset level, clear callbacks
int liquid_logger_reset(liquid_logger _q)
    { return liquid_error(LIQUID_EICONFIG,"compile-time logging disabled"); }

// print logger information to stdout
int liquid_logger_print(liquid_logger _q)
    { return liquid_error(LIQUID_EICONFIG,"compile-time logging disabled"); }

// set log level; any value below this will not be logged
int liquid_logger_set_level(liquid_logger q, int _level)
    { return liquid_error(LIQUID_EICONFIG,"compile-time logging disabled"); }

// set output configuration
int liquid_logger_set_config(liquid_logger q, int _config)
    { return liquid_error(LIQUID_EICONFIG,"compile-time logging disabled"); }

// add lock function with context
int liquid_logger_set_lock(liquid_logger        _q,
                           liquid_lock_callback _callback,
                           void *               _context)
    { return liquid_error(LIQUID_EICONFIG,"compile-time logging disabled"); }

// add callback with context
int liquid_logger_add_callback(liquid_logger       _q,
                               liquid_log_callback _callback,
                               void *              _context,
                               int                 _level)
    { return liquid_error(LIQUID_EICONFIG,"compile-time logging disabled"); }

// add file pointer for which to append logs; when file is closed, the callback
int liquid_logger_add_file(liquid_logger _q,
                           FILE *        _fid,
                           int           _level)
    { return liquid_error(LIQUID_EICONFIG,"compile-time logging disabled"); }

// open file for appending logs, returning pointer to file handle (or NULL upon
// error); when file is closed, the callback will cease appending to the file
FILE * liquid_logger_add_filename(liquid_logger _q,
                                  const char*   _filename,
                                  int           _level)
{
    liquid_error(LIQUID_EICONFIG,"compile-time logging disabled");
    return NULL;
}

// get the number of callbacks currently used
unsigned int liquid_logger_get_num_callbacks(liquid_logger q)
{
    liquid_error(LIQUID_EICONFIG,"compile-time logging disabled");
    return 0;
}

// append a log message
int liquid_log(liquid_logger _q, int _level, const char * _file,
               int _line, const char * _format, ...)
{
    if (_level < LIQUID_INFO)
        return LIQUID_OK;
    va_list ap;
    va_start(ap, _format);
    int rv = liquid_vlog(_q, _level, _file, _line, _format, ap);
    va_end(ap);
    return rv;
}

// append a log message with variable arguments
int liquid_vlog(liquid_logger _q, int _level, const char * _file,
                int _line, const char * _format, va_list _ap)
{
    vprintf(_format, _ap);
    printf("\n");
    return LIQUID_OK;
}

#endif
