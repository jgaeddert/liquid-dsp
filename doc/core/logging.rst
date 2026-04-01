
.. _logging:

Logging
=======

|liquid| includes a baseline logging capability to allow run-time control of
terminal output (level, formatting, etc.).
Logging functionality is heavily influenced by
`https://github.com/rxi/log.c <https://github.com/rxi/log.c>`_
and includes the following features:

* Minimal overhead
* Ability to write to files and/or custom callbacks
* `variadic arguments <https://en.cppreference.com/w/c/language/variadic.html>`_
* Level selectable dynamically at run-time
* Thread-safe
* Custom logging callbacks

|liquid| logging includes the following additional features:

* Customizable formatting (color, file, line number, date/timestamp)
* Compile-time ability to remove capability for speed improvements
* Internal static logger used for tracing specific to |liquid|
* Integration with :ref:`error` to log errors with the library
* Counters for tracking number of log events of each type
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
System logging in |liquid| works with a global internal :api:`liquid_logger` object
that is instantiated as part of the shared library
(see `ref:`thread_safe_logging` on how to ensure thread-safe operation).
Functions within |liquid| use this to internally log status information,
warnings, errors etc. as your program executes.
At run time, you may specify the logging level for which these messages
get logged.
For example, setting a threshold of ``LIQUID_WARN`` means that messages
with ``LIQUID_TRACE`` are not logged;
however there is a small amount of overhead to perform this check which
can be undesirable for applications where every ounce of performance
from the processor is needed.
To support this, there are several ``cmake`` options that enable disabling
logging at compile time.

If you want to disable the logging framework completely
you cand do this with the ``cmake`` flag ``-D ENABLE_LOGGING=OFF``.
Log events with levels below ``LIQUID_INFO`` are ignored.
Log events with levels at ``LIQUID_INFO`` or higher are printed to ``stdout``
without any formatting.

If you don't want logging values below a certain threshold to even be compiled, you
can specify the minimum level with the ``cmake`` flag ``LOGGING_LEVEL``.
For example, if you want to remove code for logging "trace" and "debug" levels, you
can run ``cmake -D LOGGING_LEVEL=info``.
Log events below ``LIQUID_INFO`` are not even compiled as part of the library.

If your terminal does not support color output, you can explicitly disable all ANSI
color flags
with the ``cmake`` flag ``-D ENABLE_COLOR=OFF``.


Run-Time Formatting Options
---------------------------

Formatting for logging is typically configured at run time using a
bit field. The output log includes four sections with custom
formatting for each:

.. code-block::

    <date and time> <log level> <file and line> <custom message>


Date and Time Options
^^^^^^^^^^^^^^^^^^^^^

Timestamps are formatted based on absolute date and/or time values.
Using the `C11 <https://en.cppreference.com/w/c/11.html>`_ standard
permit using the higher resolution
`timespec <https://en.cppreference.com/w/c/chrono/timespec.html>`_
object.
Choose one of the base format options:

``LIQUID_LOG_RAWTIME``
    Display the raw timestamp in terms of seconds from :c:`time(NULL);`
    Example: ``1771769351``

``LIQUID_LOG_DATETIME``
    Standard datetime using strftime and "%Y-%m-%d %T" format.
    Example: ``2026-02-22 08:18:35``

``LIQUID_LOG_DATE``
    Log just the date using strftime and "%Y-%m-%d" format.
    Example: ``2026-02-22``

``LIQUID_LOG_TIME``
    Log just the time using strftime and "%T" format.
    Example: ``08:18:35``


Use any of the following additional options in conjunction with those above:

``LIQUID_LOG_UTC``
    Log using
    `coordinated universal time <https://en.wikipedia.org/wiki/Coordinated_Universal_Time>`_
    (UTC) for date and/or time stamps. For example,
    ``LIQUID_LOG_DATETIME | LIQUID_LOG_UTC`` yields 
    ``2026-02-22T13:18:35Z``.

``LIQUID_LOG_MS``
    Include milliseconds to the end of a timestamp if requested.
    Example: ``08:18:35.123``

``LIQUID_LOG_US``
    Include microseconds to the end of a timestamp if requested.
    Example: ``08:18:35.123456``

``LIQUID_LOG_NS``
    Include nanoseconds to the end of a timestamp if requested.
    Example: ``08:18:35.123456789``

These options can be used in conjunction with one another to customize formatting.
For example:

.. code-block:: c

    liquid_logger_config(NULL, LIQUID_LOG_RAWTIME | LIQUID_LOG_MS);
    liquid_log_info("custom timestamp");
    // 1771769351.445 [info ] custom timestamp


Log Level Options
^^^^^^^^^^^^^^^^^

You can specify the formatting of the log level by choosing one of the
following:

``LIQUID_LOG_LEVEL_FULL``
    Log the full level string.
    Example: ``[warning]``

``LIQUID_LOG_LEVEL_SHORT``
    Log a shortened version of the level, truncated to 5 characters.
    Example: ``[warn ]``

``LIQUID_LOG_LEVEL_ONE``
    Log a one-character representation of the level.
    Example: ``[W]``

``LIQUID_LOG_LEVEL_NUMBER``
    Log a numeral representation of the level.
    Example: ``[3]``

Use any of the following additional options in conjunction with those above:

``LIQUID_LOG_LEVEL_BRACKETS``
    Add brackets around the log level.
    Example: ``[trace]``


File and Line Number Options
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You can print the filename by choosing one of the following methods for truncation:

``LIQUID_LOG_FILENAME``
    Log the full local filename to the screen, regardless of its length.
    Example: ``source_file.c``

``LIQUID_LOG_FILENAME_SHORT``
    Log just the local file ensuring a 20-character width (truncated and/or padded).
    Example: ``       source_file.c``

``LIQUID_LOG_FILENAME_TRUNC``
    Log just the local file ensuring a 12-character width (truncated and/or padded).
    ``…urce_file.c``

Optionally you can enable displaying the line number:

``LIQUID_LOG_LINE``
    Include the line number.
    Example: ``/path/to/my/source/file.c:123``


Color Options
^^^^^^^^^^^^^

If you want to support color output, use the following:

``LIQUID_LOG_COLOR``
    Enable color output.

Note that you can also set custom "colors" (ANSI formatting) within the object itself.


Presets
^^^^^^^

``LIQUID_LOG_COMPACT``
    Compact representation with minimal detail.
    Example:
    ``09:25:46 [I] message with (2) value``

``LIQUID_LOG_SHORT``
    Short detail (default configuration). ``LIQUID_LOG_DEFAULT`` maps to this.
    Example:
    ``08:18:35 [info ] message with (2) value``

``LIQUID_LOG_MEDIUM``
    Medium detail, showing date, time, filename, and line.
    Example:
    ``2026-02-22 08:18:35 [info ] …les/logging_extensive_example.c:29: message with (2) value``

``LIQUID_LOG_FULL``
    Full detail with as much information as available
    ``2026-02-22 08:18:35.123 [info] /path/to/liquid-dsp/logging-dev/examples/logging_extensive_example.c:29: message with (2) value``

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


.. _thread_safe_logging:

Thread-safe Operation
---------------------

The logging object in |liquid| does not include mutual exclusions (mutexes)
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

    #include <pthread.h>
    #include <stdbool.h>
    #include <liquid/liquid.h>

    // user-defined lock function
    int lock_callback(int _lock, void * _context)
    {
        pthread_mutex_t * mutex = (pthread_mutex_t*)_context;
        if (_lock)
            pthread_mutex_lock(mutex);
        else
            pthread_mutex_unlock(mutex);
        return LIQUID_OK;
    }

    int main()
    {
        // create mutex
        pthread_mutex_t mutex;
        pthread_mutex_init(&mutex,NULL);

        // set lock function callback
        liquid_logger_set_lock(NULL, lock_callback, &mutex);

        // run as necessary across as many threads as needed
        {
            liquid_log_info("logging an event");
        }

        // clean it up
        pthread_mutex_destroy(&mutex);
        return 0;
    }

An interactive extensive example can be found in
:file:`examples/logging_spinlock_example.c`.

Interface
---------

.. todo:: import public logging interface

