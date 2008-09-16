//
//
//


typedef struct port_s * port;

port port_create(unsigned int _n);

void port_destroy(port _p);

void port_print(port _p);

void port_produce(port _p, float * _v, unsigned int _n);

void port_consume(port _p, float ** _v, unsigned int _n);

void port_release(port _p, unsigned int _n);


