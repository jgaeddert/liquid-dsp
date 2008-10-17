//
// Buffer macros
//   Defines macros to be used in buffer.c for efficient
//   code reuse.  Macros are defined as follows:
//     X : name-mangling macro
//     T : data type
//     BUFFER_PRINT_LINE : for printing formatted text
//
//   The following steps need to be taken to add a new
//   buffer type "tbuffer" of type "mytype":
//     1. Modify buffer.h and buffer_internal.h to add
//        new APIs:
//          #define BUFFER_MANGLE_MYTYPE(name) LIQUID_CONCAT(tbuffer, name)
//          LIQUID_BUFFER_DEFINE_API(BUFFER_MANGLE_TYPE, mytype)
//          LIQUID_BUFFER_DEFINE_INTERNAL_API(BUFFER_MANGLE_TYPE, mytype)
//     2. Modify this file (buffer_macros.h) and define
//          #define X(name) CONCAT(tbuffer, name)
//          #define T mytype
//          #define BUFFER_PRINT_LINE(B,I) printf(...);
//     3. Create a new file "tbuffer.c" to include
//          #define BUFFER_TYPE_MYTYPE
//          #include "buffer_macros.h"
//          #include "buffer.c"
//     4. Modify the Makefile to include the target tbuffer.o
//

#ifndef __LIQUID_BUFFER_MACROS_H__
#define __LIQUID_BUFFER_MACROS_H__

#define CONCAT(prefix, name) prefix ## name

//
// Float buffer
//
#if defined(BUFFER_TYPE_FLOAT)
#   define X(name) CONCAT(fbuffer, name)
#   define T float
#   define BUFFER_PRINT_LINE(B,I) \
        printf("\t: %f", buffer_fast_access(B,I));
//
// Complex float buffer
//
#elif defined(BUFFER_TYPE_CFLOAT)
#   define X(name) CONCAT(cfbuffer, name)
#   define T float complex
#   define BUFFER_PRINT_LINE(B,I) \
        printf("\t: %f + %f", \
            crealf(buffer_fast_access(B,I)), \
            cimagf(buffer_fast_access(B,I)));
#else
#   error "invalid buffer type defined"
#endif

#endif // __LIQUID_BUFFER_MACROS_H__
