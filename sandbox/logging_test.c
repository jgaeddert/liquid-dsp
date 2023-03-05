// This example demonstrates logging capabilities with liquid
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>

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

//#define LIQUID_LOGGER_MAX_CALLBACKS (32)
struct liquid_logger_s {
    int     level;
    void * context;
    //int timezone;
    char time_fmt[16];    // format of time to pass to strftime, e.g. "%T"
    liquid_log_callback callbacks[32];
};
//void liquid_logger_set_time_fmt(liquid_logger q, const char * fmt);

static struct liquid_logger_s qlog = {
    .level   = 0,
    .context = NULL,
    .time_fmt= "%F-%T",
};

const char * liquid_log_colors[] = {"\x1b[94m","\x1b[36m","\x1b[32m","\x1b[33m","\x1b[31m","\x1b[35m"};

const char * liquid_log_levels[] = { "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL" };

enum { LIQUID_TRACE=0, LIQUID_DEBUG, LIQUID_INFO, LIQUID_WARN, LIQUID_ERROR, LIQUID_FATAL };

void liquid_log(liquid_logger q, int level, const char * file, int line, const char * format, ...);

#define liquid_log_trace(...) liquid_log(NULL,LIQUID_TRACE,__FILE__,__LINE__,__VA_ARGS__)

int main(int argc, char*argv[])
{
    int level;
    for (level=0; level<=LIQUID_FATAL; level++) {
        liquid_log(NULL,level,__FILE__,__LINE__,"message with (%d) value", level);
        usleep(100000);
    }

    //liquid_log(LIQUID_LOG_ERROR, "logger could not allocate memory for stack");
    //liquid_log_error("logger could not allocate memory for stack");

    liquid_log_trace("testing trace logging with narg=%u", 42);

    return 0;
}

void liquid_log(liquid_logger q,
                int           level,
                const char *  file,
                int           line,
                const char *  format,
                ...)
{
    if (q == NULL)
        q = &qlog;

    char time_str[80];
    time_t t = time(NULL);
    strftime(time_str, sizeof(time_str), q->time_fmt, localtime(&t));
    printf("[%s] %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m",
        time_str, liquid_log_colors[level], liquid_log_levels[level],__FILE__, __LINE__);
    //vfprintf(ev->udata, ev->fmt, ev->ap);
    va_list args;      // variadic function arguments
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}

