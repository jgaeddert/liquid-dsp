// This example demonstrates logging capabilities with liquid,
// heavily influenced by: https://github.com/rxi/log.c
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "liquid.internal.h"

typedef struct liquid_log_event_s * liquid_log_event;
typedef struct liquid_logger_s    * liquid_logger;

struct liquid_log_event_s
{
    va_list      args;      // variadic function arguments
    const char * format;    // message format
    const char * file;      // source file name
    unsigned int line;      // source line number
    int          level;     // log level
    struct tm *  timestamp; // timestamp of event
    char         time_str[64];  // formatting time buffer
};

void liquid_log_event_init(liquid_log_event event) {}

// callback function
typedef int (*liquid_log_callback)(liquid_log_event event, void * context);

#define LIQUID_LOGGER_MAX_CALLBACKS (32)
struct liquid_logger_s {
    int     level;
    //int timezone;
    char time_fmt[16];    // format of time to pass to strftime, default:"%F-%T"
    liquid_log_callback callbacks[LIQUID_LOGGER_MAX_CALLBACKS];
    void * context[LIQUID_LOGGER_MAX_CALLBACKS];

    int count[6];       // counters showing number of events of each type
};

liquid_logger liquid_logger_create();
int liquid_logger_destroy(liquid_logger _q);
int liquid_logger_reset  (liquid_logger _q);
int liquid_logger_print  (liquid_logger _q);
int liquid_logger_set_time_fmt(liquid_logger q, const char * fmt);
int liquid_logger_add_callback(liquid_logger q, liquid_log_callback _callback, void * _context);
unsigned int liquid_logger_get_num_callbacks(liquid_logger q);
int liquid_log(liquid_logger q, int level, const char * file, int line, const char * format, ...);

// global logger
static struct liquid_logger_s qlog = {
    .level   = 0,
    .time_fmt= "%F-%T",
    .callbacks = {NULL,},
    .count     = {0,0,0,0,0,0,},
};

const char * liquid_log_colors[] = {"\033[94m","\033[36m","\033[32m","\033[33m","\033[31m","\033[35m"};

const char * liquid_log_levels[] = { "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL" };

enum { LIQUID_TRACE=0, LIQUID_DEBUG, LIQUID_INFO, LIQUID_WARN, LIQUID_ERROR, LIQUID_FATAL };

#define liquid_log_trace(...) liquid_log(NULL,LIQUID_TRACE,__FILE__,__LINE__,__VA_ARGS__)
#define liquid_log_debug(...) liquid_log(NULL,LIQUID_DEBUG,__FILE__,__LINE__,__VA_ARGS__)
#define liquid_log_info(...)  liquid_log(NULL,LIQUID_INFO, __FILE__,__LINE__,__VA_ARGS__)
#define liquid_log_warn(...)  liquid_log(NULL,LIQUID_WARN, __FILE__,__LINE__,__VA_ARGS__)
#define liquid_log_error(...) liquid_log(NULL,LIQUID_ERROR,__FILE__,__LINE__,__VA_ARGS__)
#define liquid_log_fatal(...) liquid_log(NULL,LIQUID_FATAL,__FILE__,__LINE__,__VA_ARGS__)

// user-defined callback
int test_callback(liquid_log_event event, void * context)
    { printf("  custom callback invoked! (%s)\n", event->time_str); return 0; }

int main(int argc, char*argv[])
{
    int level;
    for (level=0; level<=LIQUID_FATAL; level++) {
        liquid_log(NULL,level,__FILE__,__LINE__,"message with (%d) value", level);
        usleep(100000);
    }

    //liquid_log(LIQUID_LOG_ERROR, "logger could not allocate memory for stack");
    //liquid_log_error("logger could not allocate memory for stack");

    // test macro
    liquid_log_trace("testing trace logging with narg=%u", 42);
    liquid_logger_print(&qlog);

    // test logging with custom object
    printf("\ntesting custom log:\n");
    liquid_logger custom_log = liquid_logger_create();
    custom_log->level = LIQUID_DEBUG;
    liquid_logger_add_callback(custom_log, test_callback, NULL);
    liquid_log(custom_log,LIQUID_ERROR,__FILE__,__LINE__,"could not allocate memory for %u bytes", 1024);
    liquid_logger_print(custom_log);
    liquid_logger_destroy(custom_log);

    return 0;
}

// internal methods
liquid_logger liquid_logger_safe_cast(liquid_logger _q)
    { return _q == NULL ? &qlog : _q; }

// file callback
int file_callback(liquid_log_event event, void * context)
{
    char time_str[80];
    time_str[strftime(time_str, sizeof(time_str), "%F-%T", event->timestamp)] = '\0';
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
    //_q = liquid_logger_safe_cast(_q);
    _q->level = LIQUID_WARN;
    liquid_logger_set_time_fmt(_q, "%F-%T");
    _q->callbacks[0] = NULL; // effectively reset all callbacks
    int i;
    for (i=0; i<6; i++)
        _q->count[i] = 0;
    return LIQUID_OK;
}

int liquid_logger_print(liquid_logger _q)
{
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

int liquid_logger_set_time_fmt(liquid_logger _q,
                               const char * _fmt)
{
    // TODO: validate copy operation was successful
    strncpy(_q->time_fmt, _fmt, sizeof(_q->time_fmt));
    return LIQUID_OK;
}

int liquid_logger_add_callback(liquid_logger       _q,
                               liquid_log_callback _callback,
                               void *              _context)
{
    unsigned int index = liquid_logger_get_num_callbacks(_q);

    // check that maximum has not been reached already
    if (index==LIQUID_LOGGER_MAX_CALLBACKS)
        return liquid_error(LIQUID_EICONFIG,"maximum number of callbacks (%u) reached", LIQUID_LOGGER_MAX_CALLBACKS);

    // set callback and context at this index
    _q->callbacks[index] = _callback;
    _q->context  [index] = _context;

    // assign next position NULL pointer, assuming not already full
    if (index < LIQUID_LOGGER_MAX_CALLBACKS-1)
        _q->callbacks[index+1] = NULL;

    return LIQUID_OK;
}

unsigned int liquid_logger_get_num_callbacks(liquid_logger _q)
{
    // first get index of NULL
    unsigned int i;
    for (i=0; i<LIQUID_LOGGER_MAX_CALLBACKS; i++) {
        if (_q->callbacks[i] == NULL)
            break;
    }
    return i;
}

int liquid_logger_callback_stdout(liquid_log_event _event,
                                  FILE * restrict  _stream)
{
    fprintf(_stream,"[%s] %s%-5s\033[0m \033[90m%s:%d:\033[0m",
        _event->time_str,
        liquid_log_colors[_event->level],
        liquid_log_levels[_event->level],
        _event->file,
        _event->line);

    // parse variadic function arguments
    vfprintf(_stream, _event->format, _event->args);
    fprintf(_stream,"\n");

    return LIQUID_OK;
}

int liquid_log(liquid_logger _q,
               int           _level,
               const char *  _file,
               int           _line,
               const char *  _format,
               ...)
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

    // TODO: lock

    // output to stdout
    if (_level >= _q->level) {
        va_start(event.args, _format);
        liquid_logger_callback_stdout(&event, stderr);
        va_end(event.args);
    }

    // invoke callbacks
    int i;
    for (i=0; i<LIQUID_LOGGER_MAX_CALLBACKS && _q->callbacks[i] != NULL; i++) {
        va_start(event.args, _format);
        _q->callbacks[i](&event, _q->context[i]);
        va_end(event.args);
    }

    // TODO: unlock
    return LIQUID_OK;
}

