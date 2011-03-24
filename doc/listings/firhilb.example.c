// file: doc/listings/firhilb.example.c

int main() {
    unsigned int m=5;           // filter semi-length
    float slsl=60.0f;           // filter sidelobe suppression level

    // create Hilbert transform objects
    firhilb q0 = firhilb_create(m,slsl);
    firhilb q1 = firhilb_create(m,slsl);

    float complex x;    // interpolator input
    float y[2];         // interpolator output
    float complex z;    // decimator output

    // ...

    // execute transforms
    firhilb_interp_execute(q0, x, y);   // interpolator
    firhilb_decim_execute(q1, y, &z);   // decimator

    // clean up allocated memory
    firhilb_destroy(q0);
    firhilb_destroy(q1);
}
