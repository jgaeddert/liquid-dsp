//
//
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
