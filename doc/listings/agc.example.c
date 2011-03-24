// file: doc/listings/agc.example.c

int main() {
    agc_rrrf q = agc_rrrf_create();     // create object
    agc_rrrf_set_target(q,1.0f);        // set target energy level
    agc_rrrf_set_bandwidth(q,1e-3f);    // set loop filter bandwidth

    float x;                            // input sample
    float y;                            // output sample

    // ...

    agc_rrrf_execute(q, x, &y);         // repeat as necessary

    agc_rrrf_destroy(q);                // clean it up
}
