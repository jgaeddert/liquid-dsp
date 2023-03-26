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
    struct tm *  timestamp; // timestamp of event
    void *       context;   // user-defined context
    int          level;     // log level
};

void liquid_log_event_init(liquid_log_event event) {}

// callback function
typedef int (*liquid_log_callback)(liquid_log_event event, void * context);

#define LIQUID_LOGGER_MAX_CALLBACKS (32)
struct liquid_logger_s {
    int     level;
    void * context;
    //int timezone;
    char time_fmt[16];    // format of time to pass to strftime, default:"%F-%T"
    liquid_log_callback callbacks[LIQUID_LOGGER_MAX_CALLBACKS];

    int count[6];       // counters showing number of events of each type
};

liquid_logger liquid_logger_create();
int liquid_logger_destroy(liquid_logger _q);
int liquid_logger_reset  (liquid_logger _q);
int liquid_logger_print  (liquid_logger _q);
int liquid_logger_set_time_fmt(liquid_logger q, const char * fmt);
int liquid_logger_set_context (liquid_logger q, void * context);
int liquid_logger_add_callback(liquid_logger q, liquid_log_callback cb);
unsigned int liquid_logger_get_num_callbacks(liquid_logger q);
int liquid_log(liquid_logger q, int level, const char * file, int line, const char * format, ...);

// global logger
static struct liquid_logger_s qlog = {
    .level   = 0,
    .context = NULL,
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

// test callback
int callback(liquid_log_event event, void * context)
    { printf("callback invoked!\n"); return 0; }

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
    liquid_logger_add_callback(custom_log, callback);
    liquid_log(custom_log,LIQUID_ERROR,__FILE__,__LINE__,"could not allocate memory for %u bytes", 1024);
    liquid_logger_print(custom_log);
    liquid_logger_destroy(custom_log);

    return 0;
}

// internal methods
liquid_logger liquid_logger_safe_cast(liquid_logger _q)
    { return _q == NULL ? &qlog : _q; }

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
    _q->context = NULL;
    liquid_logger_set_time_fmt(_q, "%F-%T");
    _q->callbacks[0] = NULL; // effectively reset all callbacks
    int i;
    for (i=0; i<6; i++)
        _q->count[i] = 0;
    return LIQUID_OK;
}

int liquid_logger_print(liquid_logger _q)
{
    printf("<liquid_logger, level:%s, callbacks:%u, context:%s, fmt:%s, count:",
        // TODO: validate
        liquid_log_levels[_q->level],
        liquid_logger_get_num_callbacks(_q),
        _q->context == NULL ? "null" : "set",
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

int liquid_logger_set_context(liquid_logger _q, void * _context)
{
    _q->context = _context;
    return LIQUID_OK;
}

int liquid_logger_add_callback(liquid_logger _q, liquid_log_callback _callback)
{
    unsigned int index = liquid_logger_get_num_callbacks(_q);

    // check that maximum has not been reached already
    if (index==LIQUID_LOGGER_MAX_CALLBACKS)
        return liquid_error(LIQUID_EICONFIG,"maximum number of callbacks (%u) reached", LIQUID_LOGGER_MAX_CALLBACKS);

    // set callback to this index
    _q->callbacks[index] = _callback;

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

int liquid_log(liquid_logger q,
               int           level,
               const char *  file,
               int           line,
               const char *  format,
               ...)
{
    if (q == NULL)
        q = &qlog;

    // validate level
    if (level < 0 || level >= 6)
        return liquid_error(LIQUID_EIRANGE,"log level (%d) out of range", level);

    // update count
    q->count[level]++;

    char time_str[80];
    time_t t = time(NULL);
    strftime(time_str, sizeof(time_str), q->time_fmt, localtime(&t));
    printf("[%s] %s%-5s\033[0m \033[90m%s:%d:\033[0m",
        time_str, liquid_log_colors[level], liquid_log_levels[level],__FILE__, __LINE__);

    // parse variadic function arguments
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    printf("\n");
    // invoke callbacks
    // TODO: actually pass event properly...
    int i;
    for (i=0; i<LIQUID_LOGGER_MAX_CALLBACKS; i++) {
        if (q->callbacks[i] == NULL)
            break;
        
        //
        q->callbacks[i](NULL, NULL);
    }
    return LIQUID_OK;
}

