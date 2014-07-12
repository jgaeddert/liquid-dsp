/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
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

//
// Run-time library version numbers
//

#include <assert.h>
#include <string.h>

#include "liquid.internal.h"

const char liquid_version[] = LIQUID_VERSION;

const char * liquid_libversion(void)
{
    return LIQUID_VERSION;
}

int liquid_libversion_number(void)
{
    return LIQUID_VERSION_NUMBER;
}

//
// error handling
//

// filename from which error originated
char liquid_error_filename[LIQUID_ERROR_STRLEN] = "";

// line number
int liquid_error_line = -1;

// static error value
int liquid_error_value = LIQUID_OK;

// error message
char liquid_error_message[LIQUID_ERROR_STRLEN] = "";

// format error internally and print to standard output
//  _file       :   name of file (preprocessor macro)
//  _line       :   line number (preprocessor macro)
//  _code       :   error code
void liquid_format_error(const char * _file,
                         unsigned int _line,
                         int          _code)
{
    // check code
    switch (_code) {
    case LIQUID_OK:
    case LIQUID_ERROR_INVALID_CONFIGURATION:
    case LIQUID_ERROR_INPUT_VALUE_OUT_OF_RANGE:
    case LIQUID_ERROR_INPUT_DIMENSION_OUT_OF_RANGE:
    case LIQUID_ERROR_INVALID_MODE:
    case LIQUID_ERROR_UNSUPPORTED_MODE:
    case LIQUID_ERROR_OBJECT_NOT_INITIALIZED:
    case LIQUID_ERROR_INSUFFICIENT_MEMORY:
    case LIQUID_ERROR_FILE_IO:
    case LIQUID_ERROR_INTERNAL_LOGIC:
        break;
    default:
        // invalid error passed to this method; this itself is an error
        // TODO: use this method to format this error
        fprintf(stderr,"error: invalid error code passed to error format\n");
        assert(0);
    }

    // copy filename
    strncpy(liquid_error_filename, _file, LIQUID_ERROR_STRLEN-1);

    // copy line number
    liquid_error_line = _line;

    // copy error code
    liquid_error_value = _code;
}

// print error status; example:
//  error: [1] LIQUID_ERROR_INVALID_CONFIGURATION
//      src/filter/src/firfilt.c:77: firfilt_crcf_create(),
//      cutoff-frequency must be greater than zero
void liquid_error_print(void)
{
    // ignore if there is no error
    if (liquid_error_value == LIQUID_OK)
        return;

    // print code
    fprintf(stderr,"error: [%d] ", liquid_error_value);
    switch (liquid_error_value) {
    case LIQUID_ERROR_INVALID_CONFIGURATION:
        fprintf(stderr,"LIQUID_ERROR_INVALID_CONFIGURATION\n");
        break;
    case LIQUID_ERROR_INPUT_VALUE_OUT_OF_RANGE:
        fprintf(stderr,"LIQUID_ERROR_INPUT_VALUE_OUT_OF_RANGE\n");
        break;
    case LIQUID_ERROR_INPUT_DIMENSION_OUT_OF_RANGE:
        fprintf(stderr,"LIQUID_ERROR_INPUT_DIMENSION_OUT_OF_RANGE\n");
        break;
    case LIQUID_ERROR_INVALID_MODE:
        fprintf(stderr,"LIQUID_ERROR_INVALID_MODE\n");
        break;
    case LIQUID_ERROR_UNSUPPORTED_MODE:
        fprintf(stderr,"LIQUID_ERROR_UNSUPPORTED_MODE\n");
        break;
    case LIQUID_ERROR_OBJECT_NOT_INITIALIZED:
        fprintf(stderr,"LIQUID_ERROR_OBJECT_NOT_INITIALIZED\n");
        break;
    case LIQUID_ERROR_INSUFFICIENT_MEMORY:
        fprintf(stderr,"LIQUID_ERROR_INSUFFICIENT_MEMORY\n");
        break;
    case LIQUID_ERROR_FILE_IO:
        fprintf(stderr,"LIQUID_ERROR_FILE_IO\n");
        break;
    case LIQUID_ERROR_INTERNAL_LOGIC:
        fprintf(stderr,"LIQUID_ERROR_INTERNAL_LOGIC\n");
        break;
    default:
        // invalid error passed to this method; this itself is an error
        // TODO: use this method to format this error
        fprintf(stderr,"error: invalid error code passed to error print\n");
        assert(0);
    }

    // print file, line, and method information
    fprintf(stderr,"    %s:%d\n", liquid_error_filename, liquid_error_line);

    // print custom error message
    fprintf(stderr,"    %s\n", liquid_error_message);
}

