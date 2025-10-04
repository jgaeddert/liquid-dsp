
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

// argument
struct liquid_arg_s
{
    enum {
        TYPE_INT,
        TYPE_UINT,
        TYPE_FLOAT,
        TYPE_CHAR,
        //TYPE_BOOL,
        TYPE_STRING,
        TYPE_CUSTOM
    } type;
    void * ref; // pointer to reference variable
    const char * varname; // reference variable name
    char   opt;
    const char * help;

    // custom function for setting value
    int (*callback)(const char * optarg, void * ref);
};

// print help for argument
void liquid_arg_print(struct liquid_arg_s * _arg)
{
    printf(" [-%c ", _arg->opt);
    printf("<%s:", _arg->varname);
    switch (_arg->type) {
    case TYPE_INT:      printf("%d", *(int*)(_arg->ref));      break;
    case TYPE_UINT:     printf("%u", *(unsigned*)(_arg->ref)); break;
    case TYPE_FLOAT:    printf("%g", *(float*)(_arg->ref));    break;
    case TYPE_CHAR:     printf("%c", *(char*)(_arg->ref));     break;
    case TYPE_STRING:   printf("%s", (char*)(_arg->ref));      break;
    default: printf("?");
    }
    printf(">] %s\n", _arg->help);
}

// set value from input string
int liquid_arg_set(struct liquid_arg_s * _arg, const char * _optarg)
{
    if (_arg->callback != NULL)
        return _arg->callback(_optarg, _arg->ref);

    switch (_arg->type) {
    case TYPE_INT:      *(int*)(_arg->ref) = atoi(_optarg);    break;
    case TYPE_UINT:     *(unsigned*)(_arg->ref) = atoi(_optarg); break;
    case TYPE_FLOAT:    *(float*)(_arg->ref) = atof(_optarg); break;
    case TYPE_CHAR:     *(char*)(_arg->ref) = _optarg[0]; break;
    case TYPE_STRING:
    default:
        fprintf(stderr,"liquid_argparse_set('%s'), could not set from input '%s'\n",
            _arg->varname, _optarg);
        return -1;
    }
    return 0;
}

#define LIQUID_ARGPARSE_MAX_ARGS (64)
struct liquid_argparse_s
{
    // documentation string
    const char * docstr;
    struct liquid_arg_s args[LIQUID_ARGPARSE_MAX_ARGS];
    int num_args;
    char optstr[2*LIQUID_ARGPARSE_MAX_ARGS+1];
};

int liquid_argparse_append(struct liquid_argparse_s * _q,
                           const char * _type,
                           void *       _ref,
                           const char * _varname,
                           char         _opt,
                           const char * _help,
                           void *       _callback)
{
    // check if object is full
    if (_q->num_args >= LIQUID_ARGPARSE_MAX_ARGS) {
        fprintf(stderr,"liquid_argparse_append(), cannot create more than %u arguments\n",
            LIQUID_ARGPARSE_MAX_ARGS);
        exit(-1);
    }

    // check for duplicate entries
    int i;
    for (i=0; i<_q->num_args; i++) {
        if (_q->args[i].opt == _opt) {
            fprintf(stderr,"liquid_argparse_append('%s'), duplicate key '%c' already exists with variable '%s'\n",
                _varname, _q->args[i].opt, _q->args[i].varname);
            exit(-1);
        }
    }

    //printf("appending type: %s\n", _type);
    if (strcmp(_type,"int")==0)
        _q->args[_q->num_args].type = TYPE_INT;
    else if (strcmp(_type,"unsigned int")==0)
        _q->args[_q->num_args].type = TYPE_UINT;
    else if (strcmp(_type,"float")==0)
        _q->args[_q->num_args].type = TYPE_FLOAT;
    else if (strcmp(_type,"char")==0)
        _q->args[_q->num_args].type = TYPE_CHAR;
    else if (strcmp(_type,"string")==0)
        _q->args[_q->num_args].type = TYPE_STRING;
    else {
        _q->args[_q->num_args].type = TYPE_CUSTOM;
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
    _q->optstr[n++] = ':';
    _q->optstr[n++] = '\0';

    _q->num_args++;
    return 0;
}

int liquid_argparse_set(struct liquid_argparse_s * _q,
                        char _dopt,
                        const char * _optarg)
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
    struct liquid_argparse_s __parser;                                          \
    __parser.docstr = DOCSTR;                                                   \
    __parser.num_args = 0;                                                      \
    sprintf(__parser.optstr,"h");                                               \

// add...
#define liquid_argparse_add(TYPE, VAR, DEFAULT, KEY, HELP, FUNC)                \
    /* TODO: check for certain types like 'string' */                           \
    TYPE VAR = DEFAULT; /* define and declare variable */                       \
    liquid_argparse_append(&__parser, #TYPE, (void*)&VAR, #VAR,                 \
        KEY, HELP, FUNC);

// parse
#define liquid_argparse_parse(argc,argv)                                        \
    int __dopt, __i;                                                            \
    while ((__dopt = getopt(argc,argv,__parser.optstr)) != EOF) {               \
        switch (__dopt) {                                                       \
        case 'h':                                                               \
            /* TODO: wrap docstring across multiple lines */                    \
            printf("%s - %s\n", argv[0], __parser.docstr);                      \
            printf(" [-h print this help file]\n");                             \
            for (__i=0; __i<__parser.num_args; __i++)                           \
                liquid_arg_print(__parser.args + __i);                          \
            break;                                                              \
        default:                                                                \
            liquid_argparse_set(&__parser, __dopt, optarg);                     \
        }                                                                       \
    }                                                                           \

#ifdef __cplusplus
} //extern "C"
#endif // __cplusplus

#endif // __LIQUID_ARGPARSE_H__

