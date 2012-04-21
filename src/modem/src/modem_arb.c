/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Virginia Polytechnic
 *                                      Institute & State University
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

//
// modem_arb.c
//

// create arbitrary digital modem object
modem modem_create_arbitrary(liquid_float_complex * _table,
                             unsigned int _M)
{
    // strip out bits/symbol
    unsigned int m = liquid_nextpow2(_M);
    if ( (1<<m) != _M ) {
        // TODO : eventually support non radix-2 constellation sizes
        fprintf(stderr,"error: modem_create_arbitrary(), input constellation size must be power of 2\n");
        exit(1);
    }

    // create arbitrary modem object, not initialized
    modem q = modem_create_arb(m);

    // initialize object from table
    modem_arb_init(q, _table, _M);

    // return object
    return q;
}


// create an arbitrary modem object
modem modem_create_arb(unsigned int _bits_per_symbol)
{
    modem mod = (modem) malloc( sizeof(struct modem_s) );
    mod->scheme = LIQUID_MODEM_ARB;

    modem_init(mod, _bits_per_symbol);

    mod->M = mod->M;
    mod->symbol_map = (float complex*) calloc( mod->M, sizeof(float complex) );

    mod->modulate_func = &modem_modulate_arb;
    mod->demodulate_func = &modem_demodulate_arb;

    return mod;
}

// modulate arbitrary modem type
void modem_modulate_arb(modem _mod,
                        unsigned int symbol_in,
                        float complex *y)
{
    if (symbol_in >= _mod->M) {
        fprintf(stderr,"error: modulate_arb(), input symbol exceeds maximum\n");
        exit(1);
    }

    // map sample directly to output
    *y = _mod->symbol_map[symbol_in]; 
}

// demodulate arbitrary modem type
void modem_demodulate_arb(modem _mod,
                          float complex _x,
                          unsigned int * _symbol_out)
{
    //printf("modem_demodulate_arb() invoked with I=%d, Q=%d\n", x);
    
    // search for symbol nearest to received sample
    unsigned int i;
    unsigned int s=0;
    float d;            // distance
    float d_min = 0.0f; // minimum distance

    for (i=0; i<_mod->M; i++) {
        // compute distance from received symbol to constellation point
        d = cabsf(_x - _mod->symbol_map[i]);

        // retain symbol with minimum distance
        if ( i==0 || d < d_min ) {
            d_min = d;
            s = i;
        }
    }

    // set output symbol
    *_symbol_out = s;

    // re-modulate symbol and store state
    modem_modulate_arb(_mod, *_symbol_out, &_mod->x_hat);
    _mod->r = _x;
}

// create a V.29 modem object (4 bits/symbol)
modem modem_create_V29()
{
    modem mod = modem_create_arb(4);
    modem_arb_init(mod,(float complex*)modem_arb_V29,16);
    return mod;
}

// create an arb16opt (optimal 16-qam) modem object
modem modem_create_arb16opt()
{
    modem mod = modem_create_arb(4);
    modem_arb_init(mod,(float complex*)modem_arb16opt,16);
    return mod;
}

// create an arb32opt (optimal 32-qam) modem object
modem modem_create_arb32opt()
{
    modem mod = modem_create_arb(5);
    modem_arb_init(mod,(float complex*)modem_arb32opt,32);
    return mod;
}

// create an arb64opt (optimal 64-qam) modem object
modem modem_create_arb64opt()
{
    modem mod = modem_create_arb(6);
    modem_arb_init(mod,(float complex*)modem_arb64opt,64);
    return mod;
}

// create an arb128opt (optimal 128-qam) modem object
modem modem_create_arb128opt()
{
    modem mod = modem_create_arb(7);
    modem_arb_init(mod,(float complex*)modem_arb128opt,128);
    return mod;
}

// create an arb256opt (optimal 256-qam) modem object
modem modem_create_arb256opt()
{
    modem mod = modem_create_arb(8);
    modem_arb_init(mod,(float complex*)modem_arb256opt,256);
    return mod;
}

// create an arb64vt (64-qam vt logo) modem object
modem modem_create_arb64vt()
{
    modem mod = modem_create_arb(6);
    modem_arb_init(mod,(float complex*)modem_arb_vt64,64);
    return mod;
}

// initialize an arbitrary modem object
//  _mod        :   modem object
//  _symbol_map :   arbitrary modem symbol map
//  _len        :   number of symbols in the map
void modem_arb_init(modem _mod,
                    float complex *_symbol_map,
                    unsigned int _len)
{
#ifdef LIQUID_VALIDATE_INPUT
    if (_mod->scheme != LIQUID_MODEM_ARB) {
        fprintf(stderr,"error: modem_arb_init(), modem is not of arbitrary type\n");
        exit(1);
    } else if (_len != _mod->M) {
        fprintf(stderr,"error: modem_arb_init(), array sizes do not match\n");
        exit(1);
    }
#endif

    unsigned int i;
    for (i=0; i<_len; i++)
        _mod->symbol_map[i] = _symbol_map[i];

    // balance I/Q channels
    if (_mod->scheme == LIQUID_MODEM_ARB)
        modem_arb_balance_iq(_mod);

    // scale modem to have unity energy
    modem_arb_scale(_mod);

}

// initialize an arbitrary modem object on a file
//  _mod        :   modem object
//  _filename   :   name of the data file
void modem_arb_init_file(modem _mod,
                         char * _filename)
{
    // try to open file
    FILE * fid = fopen(_filename, "r");
    if (fid == NULL) {
        fprintf(stderr,"error: modem_arb_init_file(), could not open file\n");
        exit(1);
    }

    unsigned int i, results;
    float sym_i, sym_q;
    for (i=0; i<_mod->M; i++) {
        if ( feof(fid) ) {
            fprintf(stderr,"error: modem_arb_init_file(), premature EOF for '%s'\n", _filename);
            exit(1);
        }

        results = fscanf(fid, "%f %f\n", &sym_i, &sym_q);
        _mod->symbol_map[i] = sym_i + _Complex_I*sym_q;

        // ensure proper number of symbols were read
        if (results < 2) {
            fprintf(stderr,"error: modem_arb_init_file(), unable to parse line\n");
            exit(1);
        }
    }

    fclose(fid);

    // balance I/Q channels
    if (_mod->scheme == LIQUID_MODEM_ARB)
        modem_arb_balance_iq(_mod);

    // scale modem to have unity energy
    modem_arb_scale(_mod);
}

// scale arbitrary modem constellation points
void modem_arb_scale(modem _mod)
{
    unsigned int i;

    // calculate energy
    float mag, e = 0.0f;
    for (i=0; i<_mod->M; i++) {
        mag = cabsf(_mod->symbol_map[i]);
        e += mag*mag;
    }

    e = sqrtf( e / _mod->M );

    for (i=0; i<_mod->M; i++) {
        _mod->symbol_map[i] /= e;
    }
}

// balance an arbitrary modem's I/Q points
void modem_arb_balance_iq(modem _mod)
{
    float complex mean=0.0f;
    unsigned int i;

    // accumulate average signal
    for (i=0; i<_mod->M; i++) {
        mean += _mod->symbol_map[i];
    }
    mean /= (float) (_mod->M);

    // subtract mean value from reference levels
    for (i=0; i<_mod->M; i++) {
        _mod->symbol_map[i] -= mean;
    }
}

// demodulate arbitrary modem type (soft)
void modem_demodulate_soft_arb(modem _demod,
                               float complex _r,
                               unsigned int  * _s,
                               unsigned char * _soft_bits)
{
    unsigned int bps = _demod->m;
    unsigned int M   = _demod->M;

    // gamma = 1/(2*sigma^2), approximate for constellation size
    float gamma = 1.2f*_demod->M;

    unsigned int s=0;       // hard decision output
    unsigned int k;         // bit index
    unsigned int i;         // symbol index
    float d;                // distance for this symbol
    float complex x_hat;    // re-modulated symbol

    float dmin_0[bps];
    float dmin_1[bps];
    for (k=0; k<bps; k++) {
        dmin_0[k] = 4.0f;
        dmin_1[k] = 4.0f;
    }
    float dmin = 0.0f;

    for (i=0; i<M; i++) {
        // compute distance from received symbol
        x_hat = _demod->symbol_map[i];
        d = crealf( (_r-x_hat)*conjf(_r-x_hat) );

        // set hard-decision...
        if (d < dmin || i==0) {
            s = i;
            dmin = d;
        }

        for (k=0; k<bps; k++) {
            // strip bit
            if ( (s >> (bps-k-1)) & 0x01 ) {
                if (d < dmin_1[k]) dmin_1[k] = d;
            } else {
                if (d < dmin_0[k]) dmin_0[k] = d;
            }
        }
    }

    // make assignments
    for (k=0; k<bps; k++) {
        int soft_bit = ((dmin_0[k] - dmin_1[k])*gamma)*16 + 127;
        if (soft_bit > 255) soft_bit = 255;
        if (soft_bit <   0) soft_bit = 0;
        _soft_bits[k] = (unsigned char)soft_bit;
    }

    // hard decision

    // set hard output symbol
    *_s = s;

    // re-modulate symbol and store state
    modem_modulate_arb(_demod, *_s, &_demod->x_hat);
    _demod->r = _r;
}

