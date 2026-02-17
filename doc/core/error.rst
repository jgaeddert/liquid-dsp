
Error Handling
==============

|liquid| includes basic methods for handling errors in software processing
as they propagate through the library. Unlike C++ which has exceptions natively
as part of the language``           C typically relies on integer-based return values for
tracking issues as they arise. To facilitate this``           |liquid| includes a set of
common error codes for determining if algorithms and methods have run properly.

Error Codes
-----------

+-----------------------+-------------------------------------------------------------------+
| Enumeration           | Description                                                       |
+=======================+===================================================================+
| ``LIQUID_OK``         | Everything ok (no error to report)                                |
+-----------------------+-------------------------------------------------------------------+
| ``LIQUID_EINT``       | Internal logic error;                                             |
|                       | This is a bug with liquid and should be reported immediately      |
+-----------------------+-------------------------------------------------------------------+
| ``LIQUID_EIOBJ``      | Invalid object, examples:                                         |
|                       | * ``destroy()`` method called on a ``NULL`` pointer               |
+-----------------------+-------------------------------------------------------------------+
| ``LIQUID_EICONFIG``   | Invalid parameter or configuration; examples:                     |
|                       | * setting bandwidth of a filter to a negative number              |
|                       | * setting FFT size to zero                                        |
|                       | * create a spectral periodogram object with window size greater   |
|                       |   than FFT size                                                   |
+-----------------------+-------------------------------------------------------------------+
| ``LIQUID_EIVAL``      | Input out of range; examples:                                     |
|                       | * try to take log of -1                                           |
|                       | * try to create an FFT plan of size zero                          |
+-----------------------+-------------------------------------------------------------------+
| ``LIQUID_EIRANGE``    | Invalid vector length or dimension; examples                      |
|                       | * trying to refer to the 17th element of a 2 x 2 matrix           |
|                       | * trying to multiply two matrices of incompatible dimensions      |
+-----------------------+-------------------------------------------------------------------+
| ``LIQUID_EIMODE``     | Invalid mode; examples:                                           |
|                       | * try to create a modem of type ``LIQUID_MODEM_XXX`` which        |
|                       |   does not exist                                                  |
+-----------------------+-------------------------------------------------------------------+
| ``LIQUID_EUMODE``     | unsupported mode                                                  |
|                       | (e.g. ``LIQUID_FEC_CONV_V27`` with 'libfec' not installed)        |
+-----------------------+-------------------------------------------------------------------+
| ``LIQUID_ENOINIT``    | Object has not been created or properly initialized               |
|                       | * try to run ``firfilt_crcf_execute(NULL, ...)``                  |
|                       | * try to modulate using an arbitrary modem without initializing   |
|                       |   the constellation                                               |
+-----------------------+-------------------------------------------------------------------+
| ``LIQUID_EIMEM``      | Not enough memory allocated for operation; examples:              |
|                       | * try to factor 100 = 2*2*5*5 but only give 3 spaces for factors  |
+-----------------------+-------------------------------------------------------------------+
| ``LIQUID_EIO``        | File input/output; examples:                                      |
|                       | * could not open a file for writing because of insufficient       |
|                       |   permissions                                                     |
|                       | * could not open a file for reading because it does not exist     |
|                       | * try to read more data than a file has space for                 |
|                       | * could not parse line in file (improper formatting)              |
+-----------------------+-------------------------------------------------------------------+
| ``LIQUID_ENOCONV``    | Algorithm could not converge or no solution could be found        |
|                       | * try to find roots of polynomial can sometimes cause instability |
|                       | * filter design using Parks-McClellan with extremely tight        |
|                       |   constraints                                                     |
+-----------------------+-------------------------------------------------------------------+
| ``LIQUID_ENOIMP``     | Method or function declared but not implemented or disabled       |
+-----------------------+-------------------------------------------------------------------+

Compile-Time Options
--------------------


