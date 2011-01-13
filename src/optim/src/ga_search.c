/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
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
// ga_search.c
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "liquid.internal.h"

#define LIQUID_GA_SEARCH_MAX_POPULATION_SIZE (1024)
#define LIQUID_GA_SEARCH_MAX_CHROMOSOME_SIZE (32)

#define LIQUID_DEBUG_GA_SEARCH 0

// Create a simple ga_search object; parameters are specified internally
//  _get_utility        :   chromosome fitness utility function
//  _userdata           :   user data, void pointer passed to _get_utility() callback
//  _parent             :   initial population parent chromosome, governs precision, etc.
//  _minmax             :   search direction
ga_search ga_search_create(float (*_get_utility)(void*, chromosome),
                           void * _userdata,
                           chromosome _parent,
                           int _minmax)
{
    return ga_search_create_advanced(_get_utility,
                                     _userdata,
                                     _parent,
                                     _minmax,
                                     16,        // population size
                                     0.02f);    // mutation rate
}

// Create a ga_search object, specifying search parameters
//  _get_utility        :   chromosome fitness utility function
//  _userdata           :   user data, void pointer passed to _get_utility() callback
//  _parent             :   initial population parent chromosome, governs precision, etc.
//  _minmax             :   search direction
//  _population_size    :   number of chromosomes in population
//  _mutation_rate      :   probability of mutating chromosomes
ga_search ga_search_create_advanced(float (*_get_utility)(void*, chromosome),
                                    void * _userdata,
                                    chromosome _parent,
                                    int _minmax,
                                    unsigned int _population_size,
                                    float _mutation_rate)
{
    ga_search ga;
    ga = (ga_search) malloc( sizeof(struct ga_search_s) );

    if (_population_size > LIQUID_GA_SEARCH_MAX_POPULATION_SIZE) {
        fprintf(stderr,"error: ga_search_create(), population size exceeds maximum\n");
        exit(1);
    } else if (_mutation_rate < 0.0f || _mutation_rate > 1.0f) {
        fprintf(stderr,"error: ga_search_create(), mutation rate must be in [0,1]\n");
        exit(1);
    }

    // initialize public values
    ga->userdata = _userdata;
    ga->num_parameters  = _parent->num_traits;
    ga->population_size = _population_size;
    ga->mutation_rate   = _mutation_rate;
    ga->get_utility     = _get_utility;
    ga->minimize        = ( _minmax==LIQUID_OPTIM_MINIMIZE ) ? 1 : 0;

    ga->bits_per_chromosome = _parent->num_bits;

    // initialize selection size be be 25% of population, minimum of 2
    ga->selection_size = ( ga->population_size >> 2 ) < 2 ? 2 : ga->population_size >> 2;

    // allocate internal arrays
    ga->population = (chromosome*) malloc( sizeof(chromosome)*(ga->population_size) );
    ga->utility = (float*) calloc( sizeof(float), ga->population_size );

    // create optimum chromosome (clone)
    ga->c = chromosome_create_clone(_parent);

    //printf("num_parameters: %d\n", ga->num_parameters);
    //printf("population_size: %d\n", ga->population_size);
    //printf("\nbits_per_chromosome: %d\n", ga->bits_per_chromosome);

    // create population
    unsigned int i;
    for (i=0; i<ga->population_size; i++)
        ga->population[i] = chromosome_create_clone(_parent);

    // initialize population to random, preserving first chromosome
    for (i=1; i<ga->population_size; i++)
        chromosome_init_random( ga->population[i] );

    // evaluate population
    ga_search_evaluate(ga);

    // rank chromosomes
    ga_search_rank(ga);

    // set global utility optimum
    ga->utility_opt = ga->utility[0];

    // return object
    return ga;
}

// destroy a ga_search object
void ga_search_destroy(ga_search _g)
{
    unsigned int i;
    for (i=0; i<_g->population_size; i++)
        chromosome_destroy( _g->population[i] );
    free(_g->population);

    // destroy optimum chromosome
    chromosome_destroy(_g->c);

    free(_g->utility);
    free(_g);
}

// print search parameter internals
void ga_search_print(ga_search _g)
{
    printf("ga search :\n");
    printf("    num traits      :   %u\n", _g->num_parameters);
    printf("    population size :   %u\n", _g->population_size);
    printf("    selection size  :   %u\n", _g->selection_size);
    printf("    mutation rate   :   %12.8f\n", _g->mutation_rate);
    printf("population:\n");
    unsigned int i;
    for (i=0; i<_g->population_size; i++) {
        printf("%4u: [%8.4f] ", i, _g->utility[i]);
        chromosome_printf( _g->population[i] );
    }
}

// set population/selection size
void ga_search_set_population_size(ga_search _g,
                                   unsigned int _population_size,
                                   unsigned int _selection_size)
{
    // validate input
    if (_population_size < 2) {
        fprintf(stderr,"error: ga_search_set_population_size(), population must be at least 2\n");
        exit(1);
    } else if (_selection_size == 0) {
        fprintf(stderr,"error: ga_search_set_population_size(), selection size must be greater than zero\n");
        exit(1);
    } else if (_selection_size >= _population_size) {
        fprintf(stderr,"error: ga_search_set_population_size(), selection size must be less than population\n");
        exit(1);
    }

    // re-size arrays
    _g->population = (chromosome*) realloc( _g->population, _population_size*sizeof(chromosome) );
    _g->utility = (float*) realloc( _g->utility, _population_size*sizeof(float) );

    // initialize new chromosomes (copies)
    if (_population_size > _g->population_size) {

        unsigned int i;
        unsigned int k = _g->population_size-1; // least optimal

        for (i=_g->population_size; i<_population_size; i++) {
            // clone chromosome, copying internal values
            _g->population[i] = chromosome_create_clone(_g->population[k]);

            // copy utility
            _g->utility[i] = _g->utility[k];
        }
    }

    // set internal variables
    _g->population_size = _population_size;
    _g->selection_size  = _selection_size;
}

// set mutation rate
void ga_search_set_mutation_rate(ga_search _g,
                                 float _mutation_rate)
{
    if (_mutation_rate < 0.0f || _mutation_rate > 1.0f) {
        fprintf(stderr,"error: ga_search_set_mutation_rate(), mutation rate must be in [0,1]\n");
        exit(1);
    }

    _g->mutation_rate = _mutation_rate;
}

// Execute the search
//  _g              :   ga search object
//  _max_iterations :   maximum number of iterations to run before bailing
//  _target_utility :   target utility
float ga_search_run(ga_search _g,
                    unsigned int _max_iterations,
                    float _target_utility)
{
    unsigned int i=0;
    do {
        i++;
        ga_search_evolve(_g);
    } while (
        optim_threshold_switch(_g->utility[0], _target_utility, _g->minimize) &&
        i < _max_iterations);

    // return optimum utility
    return _g->utility_opt;
}

// iterate over one evolution of the search algorithm
void ga_search_evolve(ga_search _g)
{
    // Crossover
    ga_search_crossover(_g);

    // Mutation
    ga_search_mutate(_g);

    // Evaluation
    ga_search_evaluate(_g);

    // Rank
    ga_search_rank(_g);

    if ( optim_threshold_switch(_g->utility_opt,
                                _g->utility[0],
                                _g->minimize) )
    {
        // update optimum
        _g->utility_opt = _g->utility[0];

        // copy optimum chromosome
        chromosome_copy(_g->population[0], _g->c);

#if LIQUID_DEBUG_GA_SEARCH
        printf("  utility: %0.2E", _g->utility_opt);
        chromosome_printf(_g->c);
#endif
    }
}

// get optimal chromosome
//  _g              :   ga search object
//  _c              :   output optimal chromosome
//  _utility_opt    :   fitness of _c
void ga_search_getopt(ga_search _g,
                      chromosome _c,
                      float * _utility_opt)
{
    // copy optimum chromosome
    chromosome_copy(_g->c, _c);

    // copy optimum utility
    *_utility_opt = _g->utility_opt;
}

// evaluate fitness of entire population
void ga_search_evaluate(ga_search _g)
{
    unsigned int i;
    for (i=0; i<_g->population_size; i++)
        _g->utility[i] = _g->get_utility(_g->userdata, _g->population[i]);
}

// crossover population
void ga_search_crossover(ga_search _g)
{
    chromosome p1, p2;      // parental chromosomes
    chromosome c;           // child chromosome
    unsigned int threshold;

    unsigned int i;
    for (i=_g->selection_size; i<_g->population_size; i++) {
        // ensure fittest member is used at least once as parent
        p1 = (i==_g->selection_size) ? _g->population[0] : _g->population[rand() % _g->selection_size];
        p2 = _g->population[rand() % _g->selection_size];
        threshold = rand() % _g->bits_per_chromosome;

        c = _g->population[i];

        //printf("  ga_search_crossover, p1: %d, p2: %d, c: %d\n", p1, p2, c);
        chromosome_crossover(p1, p2, c, threshold);
    }
}

// mutate population
void ga_search_mutate(ga_search _g)
{
    unsigned int i;
    unsigned int index;
    for (i=0; i<_g->population_size; i++) {

        // generate random number and mutate if within mutation_rate range
        unsigned int num_mutations = 0;
        while ( randf() < _g->mutation_rate ) {
            // generate random mutation index
            index = rand() % _g->bits_per_chromosome;

            // mutate chromosome at index
            chromosome_mutate( _g->population[i], index );

            //
            num_mutations++;

            if (num_mutations == _g->bits_per_chromosome)
                break;
        }
    }
}

// rank population by fitness
void ga_search_rank(ga_search _g)
{
    unsigned int i, j;
    float u_tmp;        // temporary utility placeholder
    chromosome c_tmp;   // temporary chromosome placeholder (pointer)

    for (i=0; i<_g->population_size; i++) {
        for (j=_g->population_size-1; j>i; j--) {
            if ( optim_threshold_switch(_g->utility[j], _g->utility[j-1], !(_g->minimize)) ) {
                // swap chromosome pointers
                c_tmp = _g->population[j];
                _g->population[j] = _g->population[j-1];
                _g->population[j-1] = c_tmp;

                // swap utility values
                u_tmp = _g->utility[j];
                _g->utility[j] = _g->utility[j-1];
                _g->utility[j-1] = u_tmp;
            }
        }
    }
}

