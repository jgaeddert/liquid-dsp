
Autotest Macros
===============

Each module contains a number of autotest scripts which use pre-processor
macros for asserting the functional correctness of the source code.

* ``CONTEND_EQUALITY(x,y)`` asserts that :c:`x == y` and fails if false.
* ``CONTEND_INEQUALITY(x,y)`` asserts that :c:`x != y`.
* ``CONTEND_GREATER_THAN(x,y)`` asserts that :c:`x > y`.
* ``CONTEND_LESS_THAN(x,y)`` asserts that :c:`x < y`
* ``CONTEND_DELTA(x,y,del)`` asserts that :c:`abs(x-y) < del`
* ``CONTEND_EXPRESSION(expr)`` asserts that some expression is true.
* ``CONTEND_SAME_DATA(*x,*y,n)`` asserts that each of :c:`n`
   byte values in the arrays referenced by :c:`void * x` and :c:`void * y` are equal.
* ``AUTOTEST_PASS()`` passes unconditionally.
* ``AUTOTEST_FAIL(string)`` prints :c:`char * string` and fails
   unconditionally.
* ``AUTOTEST_WARN(string)`` simply prints a warning.

The autotest program will keep track of which tests elicit warnings and add
them to the list of unstable tests.

Here are some examples:

*  ``CONTEND_EQUALITY(1,1)`` will pass
*  ``CONTEND_EQUALITY(1,2)`` will fail

Here is an example...

.. code-block:: c

    #include <liquid/liquid.h>

    int main() {
        // blah
    }

