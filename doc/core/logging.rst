
.. _logging:

Logging
=======

|liquid| includes a baseline logging capability to allow run-time control of
terminal output (level, formatting, etc.).
Logging functionality is heavily influenced by
`https://github.com/rxi/log.c <https://github.com/rxi/log.c>`_
(including compile-time variadic arguments, callbacks, and file output),
but includes the following extensions

* Minimal overhead
* Ability to write to files and/or custom callbacks
* Level selectable dynamically at run-time
* Customizable formatting (color, file, line number, date/timestamp, variadic arguments)
* Thread-safe
* Custom logging callbacks

* Compile-time ability to remove capability for speed improvements
* Internal static logger used for tracing specific to |liquid|
* Integration with :ref:`error` to log errors with the library
* Counters for tracking number of log events of each type
* Customizable output formatting (time stamps
* Ability to create loggers separate from internal |liquid| logging


Log Levels
----------

|liquid| includes six distinct log levels.

+-----------------------+---------------------------------------------------------------+
| Enumeration           | Description                                                   |
+=======================+===============================================================+
| ``LIQUID_TRACE``      | The lowest and most granular level. Internally this is used   |
|                       | to trace algorithm logic                                      |
+-----------------------+---------------------------------------------------------------+
| ``LIQUID_DEBUG``      | Intended for basic debugging of algorithms                    |
+-----------------------+---------------------------------------------------------------+
| ``LIQUID_INFO``       | Intended for routine application operations                   |
+-----------------------+---------------------------------------------------------------+
| ``LIQUID_WARN``       | This level is intended to designate potentially harmful       |
|                       | events, indicating something unexpected occurred and might    |
|                       | require investigation.                                        |
+-----------------------+---------------------------------------------------------------+
| ``LIQUID_ERROR``      | This level designates a serious event such as an invalid      |
|                       | configuration for a new object, passing a NULL pointer to a   |
|                       | method, or an algorithm failing to converge                   |
+-----------------------+---------------------------------------------------------------+
| ``LIQUID_FATAL``      | This level indicates a catastrophic event has occurred, and   |
|                       | the application should be aborted immediately                 |
+-----------------------+---------------------------------------------------------------+

Compile-Time Options
--------------------

You can control how logging works at compile time to ensure limited processing overhead.

If you want to disable logging completely
(and just print values with levels no less than ``LIQUID_INFO``)
with the ``cmake`` flag ``-D ENABLE_LOGGING=OFF``.

.. todo:: ensure this cmake flag is used

If your terminal does not support color output, you can explicity disable all ANSI
color flags
with the ``cmake`` flag ``-D ENABLE_COLOR=OFF``.

Future options

* File path delimiter, e.g. "/", for parsing paths
* Truncte file path to local build directory. That is, instead of
  ``/home/username/src/liquid-dsp/src/core/src/logging.c`` just use
  ``liquid-dsp/src/core/logging.c``.
  There is probably a way to do this on POSIX systems with CMake
* Fixed logging format and level (no run-time decisions)


Run-Time Formatting Options
---------------------------

Formatting for logging is typically configured at run time using a
bit field.

Date and time options

.. code-block::

    LIQUID_LOG_RAWTIME
        Display the raw timestamp in terms of seconds from :c:`time(NULL);`
        Example: "1771769351"

    LIQUID_LOG_DATETIME
        Standard datetime using strftime and "%Y-%m-%d %T" format.
        Example: "2026-02-22 08:18:35"

    LIQUID_LOG_DATE
        Log just the date using strftime and "%Y-%m-%d" format.
        Example: "2026-02-22"

    LIQUID_LOG_TIME
        Log just the time using strftime and "%T" format.
        Example: "08:18:35"

    LIQUID_LOG_DATETIME_UTC
        TBD

    LIQUID_LOG_DATE_UTC
        TBD

    LIQUID_LOG_MS
        Include milliseconds to the end of a timestamp if requested.
        Example: "08:18:35.123"

    LIQUID_LOG_US
        Include microseconds to the end of a timestamp if requested.
        Example: "08:18:35.123456"

    LIQUID_LOG_NS
        Include nanoseconds to the end of a timestamp if requested.
        Example: "08:18:35.123456789"

These options can be used in conjuction with one another to customize formatting.
For example:

.. code-block:: c

    liquid_logger_config(NULL, LIQUID_LOG_RAWTIME | LIQUID_LOG_MS);
    liquid_log_info("custom timestamp");
    // 1771769351.445 [info ] custom timestamp

Color options

.. code-block::

    LIQUID_LOG_COLOR
        Enable color output.

Note that you can also set custom "colors" (ANSI formatting) within the object itself.

Level options

.. code-block::

    LIQUID_LOG_LEVEL_BRACKETS
        (TBD) Add brackets around the log level.
        Example: "[trace]"

    LIQUID_LOG_LEVEL_FULL
        Log the full level string.
        Example: "[warning]"

    LIQUID_LOG_LEVEL_SHORT
        Log a shortened version of the level, truncated to 5 characters.
        Example: "[warn ]"

    LIQUID_LOG_LEVEL_ONE
        Log a one-character representation of the level.
        Example: "[W]"

    LIQUID_LOG_LEVEL_NUMBER
        Log a numeral representation of the level.
        Example: "[3]"

File name and line number options

.. code-block::

    LIQUID_LOG_FILENAME
        Log the full filename to the screen
        Example: "/path/to/my/source/file.c"

    LIQUID_LOG_FILENAME_SHORT
        Log just the local file, assuming a "/" delimiter
        Example: "file.c"

    LIQUID_LOG_FILENAME_TRUNCATED
        Log a truncated version of the file if the name extends
        too long, e.g. "...source/file.c"

    LIQUID_LOG_LINE
        Include the line number.
        Example: "/path/to/my/source/file.c:123"

Some presets:

``LIQUID_LOG_DEFAULT`` : ...


Logging to File
---------------

It is often convenient to log events to file simultaneously with logging
to the standard output. You can conveniently add a callback to log to a
file with a log level that might differ from the main logging level
using the :c:`liquid_logger_add_file()` method:

.. code-block:: c

    // open a file for writing
    FILE * fid = fopen("debug.log","w");
    fid = liquid_logger_add_file(NULL,fid,LIQUID_DEBUG);

    // ... run some code ...

    fclose(fid); // closing the file here is fine

    // ... run some more code ...

Note that you may close the file at any point to stop logging. This will
not cause an error as the logger will check if the file is open and return
gracefully if not. This is convenient for checking only critical parts of
the code.

For convenience, you can use the :c:`liquid_logger_add_filename()` method:

.. code-block:: c

    // add logging to a file based on its filename alone
    FILE * fid = liquid_logger_add_filename(NULL,filename,LIQUID_DEBUG);

The formatting of the file will match the main object's format, but with
color disabled.


Custom Callbacks
----------------

Logging events to files are just callbacks that are invoked internally.
You can create a custom callback that you can handle separately for
additional filtering and granularity.

.. code-block:: c

    // user-defined callback
    int test_callback(liquid_log_event event, void * context, int config)
    {
        printf("  custom callback invoked! timestamp=%s, context=%s\n",
            event->time_str, (char*)context);
        return 0;
    }

    int main(int argc, char*argv[])
    {
        // register the callback
        char context[] = "custom context";
        liquid_logger_add_callback(NULL, test_callback, context, LIQUID_LEVEL_INFO);

        // log an event
        liquid_log_warning("could not allocate memory for %u bytes", 1024);

        return 0;
    }

Thread-safe Operation
---------------------

The loggging object in |liquid| does not include mutual exclusions (mutexes)
by default as it isn't certain if linking against
`pthreads <https://www.man7.org/linux/man-pages/man7/pthreads.7.html>`_
is possible;
however supporting thread-safe operation can be enabled with the use of the
:api:`liquid_logger_set_lock()` method.
This allows you to pass a custom callback to lock/unlock a mutex whenever
an event needs to be logged.
For a single event, the callback is invoked once to lock the mutex, and again
to unlock it.
This follows the design pattern from the excellent
`https://github.com/rxi/log.c <https://github.com/rxi/log.c>`_
with a basic example shown below:

.. code-block:: c

    #include <liquid/liquid.h>
    #include <stdbool.h>

    // user-defined lock function; non-atomic spin lock for demonstration
    // (replace with e.g. pthread_mutex)
    int spinlock(int _lock, void * _flag)
    {
        if (_lock) {
            // wait for unlock
            while (*((bool*)_flag))
                ; // burn your cpu to the ground
            *((bool*)_flag) = true;
        } else {
            // unlock
            *((bool*)_flag) = false;
        }
        return LIQUID_OK;
    }

    int main()
    {
        // set custom lock
        locked = false;
        liquid_logger_set_lock(NULL, spinlock, NULL);

        // run as necesssary...
        liquid_log_info("logging an event");
    }


An interactive extensive example can be found in
:file:`examples/logging_spinlock_example.c`.

