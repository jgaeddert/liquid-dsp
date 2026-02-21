
Logging
=======

|liquid| includes a baseline logging capability.

Basline features:

* Minimal overhead
* Ability to write to files and/or custom callbacks
* Level selectable dynamically at run-time
* Compile-time ability to remove capability for speed improvements
* Ability to create loggers separate from internal |liquid| logging
* Customizable formatting (color, file, line number, date/timestamp, variadic arguments)
* Thread-safe
* Custom logging callbacks
* Counters for tracking number of log events of each type

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

Run-Time Options
----------------

.. todo:: write up run-time options

