/*
 * Copyright (c) 2007, 2009, 2011 Joseph Gaeddert
 * Copyright (c) 2007, 2009, 2011 Virginia Polytechnic
 *                                Institute & State University
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
// Artificial neural network
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "liquid.experimental.h"

#define ANN(name)       LIQUID_CONCAT(ann,name)
#define ANNLAYER(name)  LIQUID_CONCAT(annlayer,name)
#define NODE(name)      LIQUID_CONCAT(node,name)
#define DOTPROD(name)   LIQUID_CONCAT(dotprod_rrrf,name)
#define T               float

#define LIQUID_ANN_MAX_NETWORK_SIZE 1024

#define DEBUG_ANN 0

struct ANN(_s) {
    // weights
    T * w;      // weights vector
    unsigned int num_weights;

    // network structure
    unsigned int * structure;
    unsigned int num_inputs;
    unsigned int num_outputs;
    unsigned int num_layers;
    unsigned int num_nodes;

    ANNLAYER() * layers;
    T * y_hat;  // internal memory

    // activation function (derivative) pointer
    ann_activation_function activation_func_hidden;
    ann_activation_function activation_func_output;

    // back-propagation training objects
    T * dw;     // gradient weights vector [num_weights]

    // ga-based training
    gasearch ga;
};

// Creates a network
ANN() ANN(_create)(unsigned int * _structure,
                   unsigned int _num_layers,
                   int _activation_func_hidden,
                   int _activation_func_output)
{
    unsigned int i;

    // validate input
    if (_num_layers < 2) {
        printf("error: ann_create(), must have at least 2 layers\n");
        exit(1);
    }

    for (i=0; i<_num_layers; i++) {
        if (_structure[i] < 1) {
            printf("error: ann_create(), layer %u has no nodes\n", i);
            exit(1);
        }
    }

    ann q;
    q = (ANN()) malloc( sizeof(struct ANN(_s)) );

    // Initialize structural variables
    q->num_layers = _num_layers;
    q->structure = (unsigned int*) malloc( sizeof(unsigned int)*_num_layers );
    for (i=0; i<q->num_layers; i++)
        q->structure[i] = _structure[i];
    q->num_inputs = q->structure[0];
    q->num_outputs = q->structure[q->num_layers-1];
    q->activation_func_hidden = _activation_func_hidden;
    q->activation_func_output = _activation_func_output;

    // Initialize weights
    q->num_weights = 2 * (q->structure[0]);
    q->num_nodes = q->structure[0];
    for (i=1; i<q->num_layers; i++) {
        q->num_weights += (q->structure[i-1]+1) * q->structure[i];
        q->num_nodes += q->structure[i];
    }

    if (q->num_weights > LIQUID_ANN_MAX_NETWORK_SIZE) {
        printf("error: ann_create(), network size exceeded\n");
        exit(1);
    }

    // allocate memory for weights, buffers
    q->w  = (T*) malloc( (q->num_weights)*sizeof(T) );
    q->dw = (T*) malloc( (q->num_weights)*sizeof(T) );
    q->y_hat = (T*) malloc( (q->num_nodes+q->num_inputs)*sizeof(T) );

    // initialize weights
    for (i=0; i<q->num_weights; i++) {
        q->w[i] = ((i%2)==0) ? 1.0 : -1.0;
        q->w[i] = (i%2 ? 1.0f : -1.0f)*0.1f*(float)i / (float)(q->num_weights);
        //q->w[i] = (i%2 ? 0.1f : -0.1f);
        q->dw[i] = 0.0f;
    }

    // create layers
    q->layers = (ANNLAYER()*) malloc((q->num_layers)*sizeof(ANNLAYER()));
    unsigned int nw = 0;
    unsigned int nx = 0;
    unsigned int ny = q->num_inputs;
    unsigned int num_inputs, num_outputs;
    unsigned int num_weights;
    int is_input_layer;
    int is_output_layer;
    int activation_func;
    for (i=0; i<q->num_layers; i++) {
        // compute number of inputs, outputs to layer
        num_inputs = (i==0) ? 1 : q->structure[i-1];
        num_outputs = q->structure[i];

        // compute number of weights in the layer
        num_weights = (num_inputs+1)*num_outputs;

        // set input/output layer flags
        is_input_layer = (i==0);
        is_output_layer = (i==q->num_layers-1);

        activation_func = is_output_layer ?
                          q->activation_func_output :
                          q->activation_func_hidden;

        // create the node layer
        q->layers[i] = ANNLAYER(_create)(q->w + nw,
                                         q->y_hat + nx,
                                         q->y_hat + ny,
                                         num_inputs,
                                         num_outputs,
                                         is_input_layer,
                                         is_output_layer,
                                         activation_func,
                                         1.0f);
        // increment counters
        nw += num_weights;
        nx += (i==0) ? q->structure[0] : q->structure[i-1];
        ny += num_outputs;
    }

    return q;
}

// clean up the network object
void ANN(_destroy)(ANN() _q)
{
    free(_q->structure);
    free(_q->w);
    free(_q->dw);
    free(_q->y_hat);

    // destroy layer objects
    unsigned int i;
    for (i=0; i<_q->num_layers; i++)
        ANNLAYER(_destroy)(_q->layers[i]);
    free(_q->layers);

    free(_q);
}

// Prints network
void ANN(_print)(ANN() _q)
{
    unsigned int i;
    printf("perceptron network : [");
    for (i=0; i<_q->num_layers; i++)
        printf("%3u",_q->structure[i]);
    printf("]\n");
    printf("    num weights : %u\n", _q->num_weights);
    printf("    num inputs  : %u\n", _q->num_inputs);
    printf("    num outputs : %u\n", _q->num_outputs);
    printf("    num nodes   : %u\n", _q->num_nodes);
    printf("    num layers  : %u\n", _q->num_layers);

    for (i=0; i<_q->num_weights; i++)
        printf("  w[%4u] = %12.8f\n", i, _q->w[i]);

    for (i=0; i<_q->num_layers; i++)
        ANNLAYER(_print)(_q->layers[i]);
}

ANN() ANN(_load_from_file)(char * _filename)
{
    FILE * fid = fopen(_filename,"r");
    if (!fid) {
        printf("error: ann_load_from_file(), cannot open \"%s\" for reading\n", _filename);
        exit(1);
    }

    char buffer[64];    // character buffer
    int retval=0;       // fscanf return value

    // first line: number of layers
    retval = fscanf(fid,"%s", buffer);   // "NUM_LAYERS:"
    unsigned int num_layers;
    retval = fscanf(fid,"%u\n", &num_layers);

    // next line: network structure
    retval = fscanf(fid,"%s", buffer);   // "STRUCTURE:"
    unsigned int i;
    unsigned int structure[num_layers];
    for (i=0; i<num_layers; i++)
        retval = fscanf(fid,"%u", &structure[i]);

    // next line: activation function (hidden layers)
    retval = fscanf(fid,"%s", buffer);   // "ACTIVATION_HIDDEN:"
    int activation_func_hidden;
    retval = fscanf(fid,"%d", &activation_func_hidden);

    // next line: activation function (output layer)
    retval = fscanf(fid,"%s", buffer);   // "ACTIVATION_OUTPUT:"
    int activation_func_output;
    retval = fscanf(fid,"%d", &activation_func_output);

    // create network object
    ANN() q = ANN(_create)(structure,
                           num_layers,
                           activation_func_hidden,
                           activation_func_output);

    // read weights from file, overloading initialized values
    retval = fscanf(fid,"%s", buffer); // "WEIGHTS:"
    for (i=0; i<q->num_weights; i++) {
        retval = fscanf(fid,"%f", &(q->w[i]));
        if (feof(fid)) {
            printf("error: ann_load_from_file(), invalid file : premature EOF\n");
            fclose(fid);
            exit(1);
        }
    }

    fclose(fid);
    return q;
}

void ANN(_save_to_file)(ANN() _q, char * _filename)
{
    FILE * fid = fopen(_filename,"w");
    if (!fid) {
        printf("error: ann_load_from_file(), cannot open \"%s\" for writing\n", _filename);
        exit(1);
    }

    // first line: number of layers
    fprintf(fid,"NUM_LAYERS: %u\n", _q->num_layers);

    // next line: network structure
    unsigned int i;
    fprintf(fid,"STRUCTURE: ");
    for (i=0; i<_q->num_layers; i++)
        fprintf(fid,"%u ", _q->structure[i]);
    fprintf(fid,"\n");

    // next line: activation functions
    fprintf(fid,"ACTIVATION_HIDDEN: %d\n", _q->activation_func_hidden);
    fprintf(fid,"ACTIVATION_OUTPUT: %d\n", _q->activation_func_output);

    // save weights from file, reading directly from network object
    fprintf(fid,"WEIGHTS:\n");
    for (i=0; i<_q->num_weights; i++)
        fprintf(fid,"%16.8e\n", _q->w[i]);

    fclose(fid);
}

// initialize weights to random (normal distribution)
void ANN(_init_random_weights)(ANN() _q)
{
    unsigned int i;
    for (i=0; i<_q->num_weights; i++)
        _q->w[i] = randnf();
}

// Evaluates the network _q at _input and stores the result in _output
void ANN(_evaluate)(ANN() _q, T * _x, T * _y)
{
    // copy input elements to head of buffer
    memmove(_q->y_hat, _x, (_q->num_inputs)*sizeof(T));

    unsigned int i;
    for (i=0; i<_q->num_layers; i++)
        ANNLAYER(_evaluate)(_q->layers[i]);

    // copy output
    memmove(_y, &_q->y_hat[_q->num_nodes + _q->num_inputs - _q->num_outputs], (_q->num_outputs)*sizeof(float));
}

// train network on pattern set
//
// _q               :   network object
// _set             :   input pattern set
// _emax            :   maximum error tolerance
// _nmax            :   maximum number of iterations
void ANN(_train_patternset)(ANN() _q,
                            patternset _set,
                            T _emax,
                            unsigned int _nmax)
{
    // validate input
    if (_set->num_inputs != _q->num_inputs) {
        fprintf(stderr,"error: ann_train_patternset(), ann and patternset input sizes do not match\n");
        exit(1);
    } else if (_set->num_outputs != _q->num_outputs) {
        fprintf(stderr,"error: ann_train_patternset(), ann and patternset output sizes do not match\n");
        exit(1);
    }

    unsigned int i, j;
    float * x;  // input pattern
    float * y;  // output pattern
    float rmse;
    FILE * fid = fopen("ann_debug.m","w");
    fprintf(fid,"%% %s : auto-generated file\n", "ann_debug.m");
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    for (i=0; i<_nmax; i++) {
        for (j=0; j<_set->num_patterns; j++) {
            x = &_set->x[j * _q->num_inputs];
            y = &_set->y[j * _q->num_outputs];

            ANN(_train_bp)(_q,x,y);
        }

        // compute error
        rmse = ANN(_compute_rmse)(_q, _set->x, _set->y, _set->num_patterns);
        fprintf(fid,"rmse(%6u) = %16.8e;\n", i+1, rmse);

        if ((i%100)==0)
            printf("%6u : %12.4e\n", i, rmse);

        // break if error is below tolerance
        if (rmse < _emax)
            break;
    }

    fprintf(fid,"\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"semilogy(rmse)\n");
    fprintf(fid,"xlabel('training epoch');\n");
    fprintf(fid,"ylabel('RMSE');\n");
    fprintf(fid,"grid on;\n");
    fclose(fid);
    printf("training results written to %s\n", "ann_debug.m");
}


// train network
//
// _q               :   network object
// _x               :   input training pattern array [_n x nx]
// _y               :   output training pattern array [_n x ny]
// _num_patterns    :   number of training patterns
// _emax            :   maximum error tolerance
// _nmax            :   maximum number of iterations
void ANN(_train)(ANN() _q,
                 T * _x,
                 T * _y,
                 unsigned int _num_patterns,
                 T _emax,
                 unsigned int _nmax)
{
    unsigned int i, j;
    float * x;
    float * y;
    float rmse;
    FILE * fid = fopen("ann_debug.m","w");
    fprintf(fid,"%% %s : auto-generated file\n", "ann_debug.m");
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    for (i=0; i<_nmax; i++) {
        for (j=0; j<_num_patterns; j++) {
            x = &_x[j * _q->num_inputs];
            y = &_y[j * _q->num_outputs];

            ANN(_train_bp)(_q,x,y);
        }

        // compute error
        rmse = ANN(_compute_rmse)(_q,_x,_y,_num_patterns);
        fprintf(fid,"rmse(%6u) = %16.8e;\n", i+1, rmse);

        if ((i%100)==0)
            printf("%6u : %12.4e\n", i, rmse);

        // break if error is below tolerance
        if (rmse < _emax)
            break;
    }

    fprintf(fid,"\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"semilogy(rmse)\n");
    fprintf(fid,"xlabel('training epoch');\n");
    fprintf(fid,"ylabel('RMSE');\n");
    fprintf(fid,"grid on;\n");
    fclose(fid);
    printf("training results written to %s\n", "ann_debug.m");
}

// Train using backpropagation
// _q       :   network object
// _x       :   input training pattern array [num_inputs x 1]
// _y       :   output training pattern array [num_outputs x 1]
void ANN(_train_bp)(ANN() _q,
                    T * _x,
                    T * _y)
{
    unsigned int i;

    // evaluate network
    T y_hat[_q->num_outputs];
    ANN(_evaluate)(_q, _x, y_hat);
#if DEBUG_ANN
    ANN(_print)(_q);
#endif

    // compute error
    T error[_q->num_outputs];
    for (i=0; i<_q->num_outputs; i++)
        error[i] = _y[i] - y_hat[i];

#if DEBUG_ANN
    // print input, output
    printf("[");
    for (i=0; i<_q->num_inputs; i++)
        printf("%12.8f", _x[i]);
    printf("] > [");
    for (i=0; i<_q->num_outputs; i++)
        printf("%12.8f (%12.8f)",_y[i], y_hat[i]);
    printf("]\n");
#endif

    // compute back-propagation error starting with last layer and
    // working backwards
    float * e;
    unsigned int n;
    for (i=0; i<_q->num_layers; i++) {
        n = _q->num_layers - i - 1;
        e = (i==0) ? error : _q->layers[n+1]->error;

#if DEBUG_ANN
        printf(">>>>> computing bp error on layer %3u\n", n);
#endif
        ANNLAYER(_compute_bp_error)(_q->layers[n], e);
    }

    // update weights
    for (i=0; i<_q->num_layers; i++) {
        ANNLAYER(_train)(_q->layers[i], 0.01f);
    }
}

// compute network root mean-square error (rmse) on
// input patterns
float ANN(_compute_rmse)(ANN() _q,
                         T * _x,
                         T * _y,
                         unsigned int _num_patterns)
{
    // for now, just compute error
    T y_hat[_q->num_outputs];
    float d, e;
    float rmse=0.0f;
    unsigned int i, j;
    for (i=0; i<_num_patterns; i++) {
        // evaluate network
        ANN(_evaluate)(_q, &_x[i*(_q->num_inputs)], y_hat);

        // compute error
        e = 0.0f;
        for (j=0; j<_q->num_outputs; j++) {
            d = y_hat[j] - _y[i*(_q->num_outputs) + j];
            e += d*d;
        }
        rmse += e / (_q->num_outputs);
    }

    rmse = sqrtf(rmse / _num_patterns);

    return rmse;
}

// ga search structure
struct ANN(_gasearch_object) {
    ANN() network;
    float * x;
    float * y;
    unsigned int num_patterns;
};

// ga search callback
float ANN(_gasearch_callback)(void * _userdata,
                               float * _v,
                               unsigned int _num_parameters)
{
    struct ANN(_gasearch_object) *q = (struct ANN(_gasearch_object)*) _userdata;

    float rmse = ANN(_compute_rmse)(q->network,
                                    q->x,
                                    q->y,
                                    q->num_patterns);

    return rmse;
}


void ANN(_train_ga)(ANN() _q,
                    T * _x,
                    T * _y,
                    unsigned int _num_patterns,
                    float _emax,
                    unsigned int _nmax)
{
#if 0
    // create search structure
    struct ANN(_gasearch_object) obj = {_q, _x, _y, _num_patterns};

    // create search object
    gasearch ga = gasearch_create((void*)&obj,
                                    _q->w,
                                    _q->num_weights,
                                    &ANN(_gasearch_callback),
                                    LIQUID_OPTIM_MINIMIZE);

    // run search
#if 0
    gasearch_run(ga,_nmax,_emax);
#else
    unsigned int i;
#define OUTPUT_FILENAME "ann_search_ga_internal_debug.m"
    FILE* fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n",OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");

    for (i=0; i<_nmax; i++) {
        gasearch_evolve(ga);

        float rmse = ANN(_compute_rmse)(_q,_x,_y,_num_patterns);

        if ((i%100)==0)
            printf("%6u : %12.4e\n", i, rmse);

        fprintf(fid,"rmse(%6u) = %16.8e;\n", i+1, rmse);
    }
    fprintf(fid,"\n");
    fprintf(fid,"semilogy(rmse,'-','LineWidth',2)\n");
    fprintf(fid,"xlabel('training epoch');\n");
    fprintf(fid,"ylabel('rmse');\n");
    fprintf(fid,"grid on;\n");
    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);
#endif

    // clean it up
    gasearch_destroy(ga);

#endif
}

