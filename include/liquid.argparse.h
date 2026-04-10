/*
 * Copyright (c) 2007 - 2025 Joseph Gaeddert
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
#ifndef __LIQUID_ARGPARSE_H__
#define __LIQUID_ARGPARSE_H__

// <liquid.argparse.h>
//
// convenience macros for argument parsing to allow for consolidation of
//   1. variable declaration and initial assignment
//   2. command-line argument parsing using getopt
//   3. brief variable description with 'help' support
//   4. limited error-handling and custom argument parsing
//
// For example:
//
// int main(int argc, char*argv[])
// {
//     // create argument parser using macros to define variables and set values from command line
//     liquid_argparse_init(__docstr__);
//     liquid_argparse_add(char*, filename, "test.out",'o', "output filename",      NULL);
//     liquid_argparse_add(bool,  verbose,     false,  'v', "enable verbose mode",  NULL);
//     liquid_argparse_add(int,   iterations,  0,      'i', "number of iterations", NULL);
//     liquid_argparse_parse(argc,argv);
//
//     // print values
//     printf("filename   = %s\n", filename);
//     printf("verbose    = %s\n", verbose ? "true" : "false");
//     printf("iterations = %d\n", iterations);
//     return 0;
// }

// C++ type mapping: char* -> const char* for string literal compatibility
// Must be outside extern "C" block
#ifdef __cplusplus
template<typename T> struct liquid_argparse_type_t { typedef T type; };
template<> struct liquid_argparse_type_t<char*> { typedef const char* type; };
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// the maximum number of arguments allowed
#define LIQUID_ARGPARSE_MAX_ARGS (64)

// common headers
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include "liquid.h"

// callback function pointer for custom type parsing
typedef int (*liquid_argparse_callback)(const char * _optarg, void * _ref);

// argument type enumeration (defined at file scope for C++ compatibility)
enum liquid_arg_type {
    TYPE_BOOL,
    TYPE_INT,
    TYPE_UINT,
    TYPE_LONG,
    TYPE_ULONG,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_CUSTOM
};

// argument
struct liquid_arg_s
{
    enum liquid_arg_type type;
    void *       ref;       // pointer to reference variable
    const char * varname;   // reference variable name
    char         opt;       // option character
    const char * help;      // help string

    // custom function for setting value
    liquid_argparse_callback callback;
};

// print help for argument
int liquid_arg_print(struct liquid_arg_s * _arg)
{
    printf(" [-%c ", _arg->opt);
    if (_arg->type != TYPE_BOOL)
        printf("<%s:", _arg->varname); // requires argument
    switch (_arg->type) {
    case TYPE_BOOL:   printf("%s", *(bool*)         (_arg->ref) ? "true": "false" ); break;
    case TYPE_INT:    printf("%d", *(int*)          (_arg->ref)); break;
    case TYPE_UINT:   printf("%u", *(unsigned*)     (_arg->ref)); break;
    case TYPE_LONG:   printf("%ld",*(long*)         (_arg->ref)); break;
    case TYPE_ULONG:  printf("%lu",*(unsigned long*)(_arg->ref)); break;
    case TYPE_FLOAT:  printf("%g", *(float*)        (_arg->ref)); break;
    case TYPE_DOUBLE: printf("%g", *(double*)       (_arg->ref)); break;
    case TYPE_CHAR:   printf("%c", *(char*)         (_arg->ref)); break;
    case TYPE_STRING: printf("%s", *(char**)        (_arg->ref)); break;
    default:
        return liquid_error(LIQUID_EINT,"unexpected argument type: %d", _arg->type);
    }
    if (_arg->type != TYPE_BOOL)
        printf(">"); // requires argument
    printf("] %s\n", _arg->help);
    return LIQUID_OK;
}

// print help for argument in JSON format
int liquid_arg_print_json(struct liquid_arg_s * _arg)
{
    printf("{\"opt\":\"%c\", ", _arg->opt);
    printf("\"varname\":\"%s\",", _arg->varname);
    switch (_arg->type) {
    case TYPE_BOOL:   printf("\"type\":\"bool\", \"value\":%s", *(bool*)                  (_arg->ref) ? "true": "false" ); break;
    case TYPE_INT:    printf("\"type\":\"int\", \"value\":%d", *(int*)                    (_arg->ref)); break;
    case TYPE_UINT:   printf("\"type\":\"unsigned\", \"value\":%u", *(unsigned*)          (_arg->ref)); break;
    case TYPE_LONG:   printf("\"type\":\"long\", \"value\":%ld",*(long*)                  (_arg->ref)); break;
    case TYPE_ULONG:  printf("\"type\":\"unsigned long\", \"value\":%lu",*(unsigned long*)(_arg->ref)); break;
    case TYPE_FLOAT:  printf("\"type\":\"float\", \"value\":%g", *(float*)                (_arg->ref)); break;
    case TYPE_DOUBLE: printf("\"type\":\"double\", \"value\":%g", *(double*)              (_arg->ref)); break;
    case TYPE_CHAR:   printf("\"type\":\"char\", \"value\":%c", *(char*)                  (_arg->ref)); break;
    case TYPE_STRING: printf("\"type\":\"char*\", \"value\":\"%s\"", *(char**)            (_arg->ref)); break;
    default:
        return liquid_error(LIQUID_EINT,"unexpected argument type: %d", _arg->type);
    }
    printf(", \"help\" : \"%s\"}", _arg->help);
    return LIQUID_OK;
}

// set value from input string
int liquid_arg_set(struct liquid_arg_s * _arg, const char * _optarg)
{
    if (_arg->callback != NULL)
        return _arg->callback(_optarg, _arg->ref);

    switch (_arg->type) {
    case TYPE_BOOL:   *(bool*)         (_arg->ref) = true;/* enable */break;
    case TYPE_INT:    *(int*)          (_arg->ref) = atoi(_optarg);   break;
    case TYPE_UINT:   *(unsigned*)     (_arg->ref) = atoi(_optarg);   break;
    case TYPE_LONG:   *(long*)         (_arg->ref) = atol(_optarg);   break;
    case TYPE_ULONG:  *(unsigned long*)(_arg->ref) = atol(_optarg);   break;
    case TYPE_FLOAT:  *(float*)        (_arg->ref) = atof(_optarg);   break;
    case TYPE_DOUBLE: *(double*)       (_arg->ref) = atof(_optarg);   break;
    case TYPE_CHAR:   *(char*)         (_arg->ref) = _optarg[0];      break;
    case TYPE_STRING: *(char**)        (_arg->ref) = (char*)_optarg;  break;
    default:
        return liquid_error(LIQUID_EICONFIG,"liquid_argparse_set('%s'), could not set from input '%s'",
            _arg->varname, _optarg);
    }
    return LIQUID_OK;
}

struct liquid_argparse_s
{
    // documentation string
    const char * docstr;
    struct liquid_arg_s args[LIQUID_ARGPARSE_MAX_ARGS];
    int num_args;
    char optstr[2*LIQUID_ARGPARSE_MAX_ARGS+1];
};

// print formatted help
int liquid_argparse_print(struct liquid_argparse_s * _q,
                          const char *               _argv0)
{
    // TODO: wrap docstring across multiple lines
    printf("%s - %s\n", _argv0, _q->docstr);
    unsigned int i;
    printf(" [-h print this help file and exit]\n");
    printf(" [-j print this help file as JSON and exit]\n");
    for (i=0; i<_q->num_args; i++)
        liquid_arg_print(_q->args + i);
    return LIQUID_OK;
}

// print formatted help as JSON
int liquid_argparse_print_json(struct liquid_argparse_s * _q,
                               const char *               _argv0)
{
    printf("{\n");
    printf("  \"application\":\"%s\",\n",_argv0);
    printf("  \"description\":\"%s\",\n",_q->docstr);
    printf("  \"options\":\n");
    printf("  [\n");
    unsigned int i;
    for (i=0; i<_q->num_args; i++) {
        printf("    ");
        liquid_arg_print_json(_q->args + i);
        printf(",\n");
    }
    printf("    {\"opt\":\"h\", \"varname\":null, \"type\":null, \"value\":false, \"help\":\"print this help file and exit\"}\n");
    printf("  ]\n");
    printf("}\n");
    return LIQUID_OK;
}

// append argument to parser
int liquid_argparse_append(struct liquid_argparse_s * _q,
                           const char *               _type,
                           void *                     _ref,
                           const char *               _varname,
                           char                       _opt,
                           const char *               _help,
                           liquid_argparse_callback   _callback)
{
    // check if object is full
    if (_q->num_args >= LIQUID_ARGPARSE_MAX_ARGS) {
        return liquid_error(LIQUID_EIMEM,"liquid_argparse_append(), cannot create more than %u arguments",
            LIQUID_ARGPARSE_MAX_ARGS);
    }

    // check for reserved keys
    if (_opt == 'h')
        return liquid_error(LIQUID_EICONFIG,"liquid_argparse_append('%s'), key 'h' is reserved for help", _varname);
    if (_opt == 'j')
        return liquid_error(LIQUID_EICONFIG,"liquid_argparse_append('%s'), key 'j' is reserved for help", _varname);

    // check for duplicate entries
    int i;
    for (i=0; i<_q->num_args; i++) {
        if (_q->args[i].opt == _opt) {
            return liquid_error(LIQUID_EICONFIG,"liquid_argparse_append('%s'), duplicate key '%c' already exists with variable '%s'",
                _varname, _q->args[i].opt, _q->args[i].varname);
        }
    }

    //printf("appending type: %s\n", _type);
    if (strcmp(_type,"bool")==0)
        _q->args[_q->num_args].type = TYPE_BOOL;
    else if (strcmp(_type,"int")==0)
        _q->args[_q->num_args].type = TYPE_INT;
    else if (strcmp(_type,"unsigned int")==0 || strcmp(_type,"unsigned")==0)
        _q->args[_q->num_args].type = TYPE_UINT;
    else if (strcmp(_type,"long")==0 || strcmp(_type,"long int")==0)
        _q->args[_q->num_args].type = TYPE_LONG;
    else if (strcmp(_type,"unsigned long")==0 || strcmp(_type,"unsigned long int")==0)
        _q->args[_q->num_args].type = TYPE_ULONG;
    else if (strcmp(_type,"float")==0)
        _q->args[_q->num_args].type = TYPE_FLOAT;
    else if (strcmp(_type,"double")==0)
        _q->args[_q->num_args].type = TYPE_DOUBLE;
    else if (strcmp(_type,"char")==0)
        _q->args[_q->num_args].type = TYPE_CHAR;
    else if (strcmp(_type,"char*")==0 || strcmp(_type,"char *")==0 || strcmp(_type,"char[]")==0)
        _q->args[_q->num_args].type = TYPE_STRING;
    else {
        _q->args[_q->num_args].type = TYPE_CUSTOM;
        if (_callback == NULL) {
            return liquid_error(LIQUID_EICONFIG,"liquid_argparse_append('%s'), callback required to handle non-standard type '%s', supported types: bool, int, unsigned int, unsigned, int, float, double, char, char*",
                _varname, _type);
        }
    }

    // add remaining elements
    _q->args[_q->num_args].ref      = _ref;
    _q->args[_q->num_args].varname  = _varname;
    _q->args[_q->num_args].opt      = _opt;
    _q->args[_q->num_args].help     = _help;
    _q->args[_q->num_args].callback = _callback;

    // extend optarg string
    int n = strlen(_q->optstr);
    _q->optstr[n++] = _opt;
    if (_q->args[_q->num_args].type != TYPE_BOOL)
        _q->optstr[n++] = ':';
    _q->optstr[n++] = '\0';

    _q->num_args++;
    return LIQUID_OK;
}

// set value according to string
int liquid_argparse_set(struct liquid_argparse_s * _q,
                        char                       _dopt,
                        const char *               _optarg)
{
    // find and set element
    int i;
    for (i=0; i<_q->num_args; i++) {
        if (_q->args[i].opt == _dopt)
            return liquid_arg_set(_q->args + i, _optarg);
    }

    return liquid_error(LIQUID_EICONFIG,"invalid option '%c'", _dopt);
}

//
// macros for simplifying interface
//

// initialize the parsing object
#define liquid_argparse_init(DOCSTR)                                            \
    /* declare parser object and initialize with input options */               \
    struct liquid_argparse_s __parser;                                          \
    __parser.docstr = DOCSTR;                                                   \
    __parser.num_args = 0;                                                      \
    snprintf(__parser.optstr, sizeof(__parser.optstr), "hj"); /* ensure 'h', 'j' are reserved for help */  \

// add option to list of arguments
// Note: In C++ mode, char* string literals must be const char*
#ifdef __cplusplus
#define liquid_argparse_add(TYPE, VAR, DEFAULT, KEY, HELP, FUNC)                \
    liquid_argparse_type_t<TYPE>::type VAR = DEFAULT;                           \
    if (liquid_argparse_append(&__parser, #TYPE, (void*)&VAR, #VAR,             \
        KEY, HELP, FUNC))                                                       \
    {                                                                           \
        return liquid_error(LIQUID_EICONFIG,"could not create argument");       \
    }
#else
#define liquid_argparse_add(TYPE, VAR, DEFAULT, KEY, HELP, FUNC)                \
    TYPE VAR = DEFAULT; /* define and declare variable */                       \
    if (liquid_argparse_append(&__parser, #TYPE, (void*)&VAR, #VAR,             \
        KEY, HELP, FUNC))                                                       \
    {                                                                           \
        return liquid_error(LIQUID_EICONFIG,"could not create argument");       \
    }
#endif                                                                           \

// parse input
#define liquid_argparse_parse(argc,argv)                                        \
    int __dopt;                                                                 \
    while ((__dopt = getopt(argc,argv,__parser.optstr)) != EOF) {               \
        switch (__dopt) {                                                       \
        case 'h':                                                               \
            exit( liquid_argparse_print(&__parser, argv[0]) );                  \
        case 'j':                                                               \
            exit( liquid_argparse_print_json(&__parser, argv[0]) );             \
        default:                                                                \
            if (liquid_argparse_set(&__parser, __dopt, optarg))                 \
                exit(-1);                                                       \
        }                                                                       \
    }                                                                           \

// callback: crc scheme - handle invalid types
int liquid_argparse_crc(const char * _optarg, void * _ref)
{
    if (liquid_getopt_str2crc(_optarg) == LIQUID_CRC_UNKNOWN)
        return liquid_error(LIQUID_EICONFIG,"unknown/unsupported crc scheme '%s'",_optarg);

    *((const char**)_ref) = _optarg;
    return 0;
}

// callback: fec scheme - handle invalid types
int liquid_argparse_fec(const char * _optarg, void * _ref)
{
    if (liquid_getopt_str2fec(_optarg) == LIQUID_FEC_UNKNOWN)
        return liquid_error(LIQUID_EICONFIG,"unknown/unsupported fec scheme '%s'",_optarg);

    *((const char**)_ref) = _optarg;
    return 0;
}

// callback: mod scheme - handle invalid types
int liquid_argparse_modem(const char * _optarg, void * _ref)
{
    if (liquid_getopt_str2mod(_optarg) == LIQUID_MODEM_UNKNOWN)
        return liquid_error(LIQUID_EICONFIG,"unknown/unsupported modulation scheme '%s'",_optarg);

    *((const char**)_ref) = _optarg;
    return 0;
}

// callback: filter - handle invalid types
int liquid_argparse_firfilt(const char * _optarg, void * _ref)
{
    if (liquid_getopt_str2firfilt(_optarg) == LIQUID_FIRFILT_UNKNOWN)
        return liquid_error(LIQUID_EICONFIG,"unknown/unsupported filter type '%s'",_optarg);

    *((const char**)_ref) = _optarg;
    return 0;
}

// callback: window - handle invalid types
int liquid_argparse_window(const char * _optarg, void * _ref)
{
    if (liquid_getopt_str2window(_optarg) == LIQUID_WINDOW_UNKNOWN)
        return liquid_error(LIQUID_EICONFIG,"unknown/unsupported window type '%s'",_optarg);

    *((const char**)_ref) = _optarg;
    return 0;
}

#ifdef __cplusplus
} //extern "C"
#endif // __cplusplus

#endif // __LIQUID_ARGPARSE_H__

