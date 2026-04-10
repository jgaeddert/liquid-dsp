char __docstr__[] = "Demonstrate logging with mutex";
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "liquid.h"
#include "liquid.argparse.h"

// lock flag
bool locked;

// user-defined lock function; non-atomic spin lock for demonstration
int spinlock(int _lock, void * _context);

// user interrupt
void signal_handler(int _signal);

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_parse(argc,argv);

    // set custom lock
    locked = false;
    liquid_logger_set_lock(NULL, spinlock, NULL);
    signal(SIGINT, &signal_handler);
    int i = 10;
    while (i--)
    {
        liquid_log_info("timer = %d; hit CTRL-C to lock/unlock",i);
        usleep(1000000);
    }
    return 0;
}

// user-defined lock function; non-atomic spin lock for demonstration
int spinlock(int _lock, void * _context)
{
    (void)_context;
    if (_lock)
    {
        while (locked)
            usleep(1000);
        locked = true;
    } else {
        locked = false;
    }
    return 0;
}

void signal_handler(int _signal)
{
    if (_signal != SIGINT)
        return;

    printf("%slocking... (hit CTRL-C to %slock)\n", locked ? "un" : "", locked ? "" : "un");

    // toggle flag; this assumes user is much slower than log function
    locked = !locked;
}

