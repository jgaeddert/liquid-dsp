//
// Buffers
//

#ifndef __MODULE_BUFFER_H__
#define __MODULE_BUFFER_H__

#include <unistd.h>
#include <complex.h>

#define LIQUID_CONCAT(prefix, name) prefix ## name

//
// Buffers
//
#define BUFFER_MANGLE_FLOAT(name)  LIQUID_CONCAT(fbuffer, name)
#define BUFFER_MANGLE_CFLOAT(name) LIQUID_CONCAT(cfbuffer, name)
#define BUFFER_MANGLE_UINT(name)   LIQUID_CONCAT(uibuffer, name)

typedef enum {
    CIRCULAR=0,
    STATIC
} buffer_type;

// large macro
//   X: name-mangling macro
//   T: data type
#define LIQUID_BUFFER_DEFINE_API(X,T)                   \
                                                        \
typedef struct X(_s) * X();                             \
X() X(_create)(buffer_type _type, unsigned int _n);     \
void X(_destroy)(X() _b);                               \
void X(_print)(X() _b);                                 \
void X(_debug_print)(X() _b);                           \
void X(_clear)(X() _b);                                 \
void X(_zero)(X() _b);                                  \
void X(_read)(X() _b, T ** _v, unsigned int *_nr);      \
void X(_release)(X() _b, unsigned int _n);              \
void X(_write)(X() _b, T * _v, unsigned int _n);        \
void X(_push)(X() _b, T _v);
//void X(_force_write)(X() _b, T * _v, unsigned int _n);

// Define buffer APIs
LIQUID_BUFFER_DEFINE_API(BUFFER_MANGLE_FLOAT, float)
LIQUID_BUFFER_DEFINE_API(BUFFER_MANGLE_CFLOAT, float complex)
LIQUID_BUFFER_DEFINE_API(BUFFER_MANGLE_UINT, unsigned int)


//
// Windows
//

#define WINDOW_MANGLE_FLOAT(name)  LIQUID_CONCAT(fwindow, name)
#define WINDOW_MANGLE_CFLOAT(name) LIQUID_CONCAT(cfwindow, name)
#define WINDOW_MANGLE_UINT(name)   LIQUID_CONCAT(uiwindow, name)

// large macro
//   X  :   name-mangling macro
//   T  :   data type
#define LIQUID_WINDOW_DEFINE_API(X,T)                   \
                                                        \
typedef struct X(_s) * X();                             \
X() X(_create)(unsigned int _n);                        \
void X(_destroy)(X() _w);                               \
void X(_print)(X() _w);                                 \
void X(_debug_print)(X() _w);                           \
void X(_clear)(X() _w);                                 \
void X(_read)(X() _w, T ** _v);                         \
void X(_push)(X() _b, T _v);                            \
void X(_write)(X() _b, T * _v, unsigned int _n);

// Define window APIs
LIQUID_WINDOW_DEFINE_API(WINDOW_MANGLE_FLOAT, float)
LIQUID_WINDOW_DEFINE_API(WINDOW_MANGLE_CFLOAT, float complex)
LIQUID_WINDOW_DEFINE_API(WINDOW_MANGLE_UINT, unsigned int)



//
// Ports
//
#define PORT_MANGLE_FLOAT(name)     LIQUID_CONCAT(fport,name)
#define PORT_MANGLE_CFLOAT(name)    LIQUID_CONCAT(cfport,name)
#define PORT_MANGLE_UINT(name)      LIQUID_CONCAT(uiport,name)

// Macro
//  X: name-mangling macro
//  T: data type
#define LIQUID_PORT_DEFINE_API(X,T) \
typedef struct X(_s) * X(); \
X() X(_create)(unsigned int _n); \
void X(_destroy)(X() _p); \
void X(_print)(X() _p); \
void X(_produce)(X() _p, T * _w, unsigned int _n); \
void X(_consume)(X() _p, T * _r, unsigned int _n); \

LIQUID_PORT_DEFINE_API(PORT_MANGLE_FLOAT, float)
LIQUID_PORT_DEFINE_API(PORT_MANGLE_CFLOAT, float complex)
LIQUID_PORT_DEFINE_API(PORT_MANGLE_UINT, unsigned int)

//
// Generic port
//
typedef struct gport_s * gport;
gport gport_create(unsigned int _n, size_t _size);
void gport_destroy(gport _p);
void gport_print(gport _p);

// producer methods
void * gport_producer_lock(gport _p, unsigned int _n);
void gport_producer_unlock(gport _p, unsigned int _n);

// consumer methods
void * gport_consumer_lock(gport _p, unsigned int _n);
void gport_consumer_unlock(gport _p, unsigned int _n);

//
// Generic port (2)
//
typedef struct gport2_s * gport2;
gport2 gport2_create(unsigned int _n, size_t _size);
void gport2_destroy(gport2 _p);
void gport2_print(gport2 _p);

// produce/consume methods
void gport2_produce(gport2 _p, void * _w, unsigned int _n);
void gport2_consume(gport2 _p, void * _r, unsigned int _n);
void gport2_consume_available(gport2 _p, void * _r, unsigned int _nmax, unsigned int *_nc);


#endif // __MODULE_BUFFER_H__

