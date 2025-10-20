
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

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// common headers
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include "liquid.h"

// the maximum number of arguments allowed
#define LIQUID_ARGPARSE_MAX_ARGS (64)

// callback function pointer for custom type parsing
typedef int (*liquid_argparse_callback)(const char * _optarg, void * _ref);

// argument
struct liquid_arg_s
{
    enum {
        TYPE_BOOL,
        TYPE_INT,
        TYPE_UINT,
        TYPE_FLOAT,
        TYPE_DOUBLE,
        TYPE_CHAR,
        TYPE_STRING,
        TYPE_CUSTOM
    } type;
    void *       ref;       // pointer to reference variable
    const char * varname;   // reference variable name
    char         opt;       // option character
    const char * help;      // help string

    // custom function for setting value
    liquid_argparse_callback callback;
};

// print help for argument
void liquid_arg_print(struct liquid_arg_s * _arg)
{
    printf(" [-%c ", _arg->opt);
    if (_arg->type != TYPE_BOOL)
        printf("<%s:", _arg->varname); // requires argument
    switch (_arg->type) {
    case TYPE_BOOL:   printf("%s", *(bool*)    (_arg->ref) ? "true": "false" ); break;
    case TYPE_INT:    printf("%d", *(int*)     (_arg->ref)); break;
    case TYPE_UINT:   printf("%u", *(unsigned*)(_arg->ref)); break;
    case TYPE_FLOAT:  printf("%g", *(float*)   (_arg->ref)); break;
    case TYPE_DOUBLE: printf("%g", *(double*)  (_arg->ref)); break;
    case TYPE_CHAR:   printf("%c", *(char*)    (_arg->ref)); break;
    case TYPE_STRING: printf("%s", *(char**)   (_arg->ref)); break;
    default: printf("?");
    }
    if (_arg->type != TYPE_BOOL)
        printf(">"); // requires argument
    printf("] %s\n", _arg->help);
}

// set value from input string
int liquid_arg_set(struct liquid_arg_s * _arg, const char * _optarg)
{
    if (_arg->callback != NULL)
        return _arg->callback(_optarg, _arg->ref);

    switch (_arg->type) {
    case TYPE_BOOL:   *(bool*)    (_arg->ref) = true;/* enable */break;
    case TYPE_INT:    *(int*)     (_arg->ref) = atoi(_optarg);   break;
    case TYPE_UINT:   *(unsigned*)(_arg->ref) = atoi(_optarg);   break;
    case TYPE_FLOAT:  *(float*)   (_arg->ref) = atof(_optarg);   break;
    case TYPE_DOUBLE: *(double*)  (_arg->ref) = atof(_optarg);   break;
    case TYPE_CHAR:   *(char*)    (_arg->ref) = _optarg[0];      break;
    case TYPE_STRING: *(char**)   (_arg->ref) = (char*)_optarg;  break;
    default:
        fprintf(stderr,"liquid_argparse_set('%s'), could not set from input '%s'\n",
            _arg->varname, _optarg);
        return -1;
    }
    return 0;
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
void liquid_argparse_print(struct liquid_argparse_s * _q,
                           const char *               _argv0)
{
    // TODO: wrap docstring across multiple lines
    printf("%s - %s\n", _argv0, _q->docstr);
    unsigned int i;
    printf(" [-h print this help file and exit]\n");
    for (i=0; i<_q->num_args; i++)
        liquid_arg_print(_q->args + i);
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
        fprintf(stderr,"liquid_argparse_append(), cannot create more than %u arguments\n",
            LIQUID_ARGPARSE_MAX_ARGS);
        return -1;
    }

    // check for duplicate entries
    int i;
    for (i=0; i<_q->num_args; i++) {
        if (_q->args[i].opt == _opt) {
            fprintf(stderr,"liquid_argparse_append('%s'), duplicate key '%c' already exists with variable '%s'\n",
                _varname, _q->args[i].opt, _q->args[i].varname);
            return -1;
        }
    }

    //printf("appending type: %s\n", _type);
    if (strcmp(_type,"bool")==0)
        _q->args[_q->num_args].type = TYPE_BOOL;
    else if (strcmp(_type,"int")==0)
        _q->args[_q->num_args].type = TYPE_INT;
    else if (strcmp(_type,"unsigned int")==0 || strcmp(_type,"unsigned")==0)
        _q->args[_q->num_args].type = TYPE_UINT;
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
            fprintf(stderr,"liquid_argparse_append('%s'), callback required to handle non-standard type '%s'\n",
                _varname, _type);
            fprintf(stderr,"  supported types: bool, int, unsigned int, unsigned, int, float, double, char, char*\n");
            return -1;
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
    return 0;
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

    fprintf(stderr,"invalid option '%c'\n", _dopt);
    return -1;
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
    sprintf(__parser.optstr,"h"); /* ensure '-h' is reserved for help */        \

// add option to list of arguments
#define liquid_argparse_add(TYPE, VAR, DEFAULT, KEY, HELP, FUNC)                \
    TYPE VAR = DEFAULT; /* define and declare variable */                       \
    if (liquid_argparse_append(&__parser, #TYPE, (void*)&VAR, #VAR,             \
        KEY, HELP, FUNC))                                                       \
    {                                                                           \
        fprintf(stderr,"%s:%u: could not create argument\n",__FILE__,__LINE__); \
        return -1;                                                              \
    }                                                                           \

// parse input
#define liquid_argparse_parse(argc,argv)                                        \
    int __dopt;                                                                 \
    while ((__dopt = getopt(argc,argv,__parser.optstr)) != EOF) {               \
        switch (__dopt) {                                                       \
        case 'h':                                                               \
            liquid_argparse_print(&__parser, argv[0]);                          \
            exit(0);                                                            \
        default:                                                                \
            if (liquid_argparse_set(&__parser, __dopt, optarg))                 \
                exit(-1);                                                       \
        }                                                                       \
    }                                                                           \

// callback: fec scheme - handle valid types
int liquid_argparse_fec(const char * _optarg, void * _ref)
{
    if (liquid_getopt_str2fec(_optarg) == LIQUID_FEC_UNKNOWN) {
        return fprintf(stderr,"error: unknown/unsupported fec scheme '%s'\n",_optarg);
    }
    return 0;
}

#ifdef __cplusplus
} //extern "C"
#endif // __cplusplus

#endif // __LIQUID_ARGPARSE_H__

