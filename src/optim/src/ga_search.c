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

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include "liquid.internal.h"

#define LIQUID_GA_SEARCH_MAX_POPULATION_SIZE (1024)
#define LIQUID_GA_SEARCH_MAX_CHROMOSOME_SIZE (32)

#define LIQUID_DEBUG_GA_SEARCH 0

// Create a simple ga_search object; parameters are specified internally
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
ga_search ga_search_create_advanced(float (*_get_utility)(void*, chromosome),
                                    void * _userdata,
                                    chromosome _parent,
                                    int _minmax,
                                    unsigned int _population_size,
                                    unsigned int _mutation_rate)
{
    ga_search ga;
    ga = (ga_search) malloc( sizeof(struct ga_search_s) );

    if (_population_size > LIQUID_GA_SEARCH_MAX_POPULATION_SIZE) {
        fprintf(stderr,"error: ga_search_create(), population_size exceeds maximum\n");
        exit(1);
    } else;

    _mutation_rate = (_mutation_rate > 0.2f) ? 0.2f : _mutation_rate;
    unsigned int total_bits = _parent->num_bits;
    unsigned int _num_mutations = (unsigned int) ceil(fabsf(_mutation_rate*total_bits));
    _num_mutations = (_num_mutations < 1 ) ? 1 : _num_mutations;
    //printf("mutation rate: %0.5f, num_mutations: %u\n\n", _mutation_rate, _num_mutations);

    // initialize public values
    ga->userdata = _userdata;
    //ga->v = _v;
    ga->num_parameters = _parent->num_traits;
    ga->population_size = _population_size;
    ga->num_mutations = _num_mutations;
    ga->get_utility = _get_utility;
    ga->minimize = ( _minmax==LIQUID_OPTIM_MINIMIZE ) ? 1 : 0;

    // initialize private values

    ga->bits_per_chromosome = _parent->num_bits;
    ga->population = (chromosome*) malloc( sizeof(struct chromosome_s)*(ga->population_size) );
    ga->utility = (float*) calloc( sizeof(float), ga->population_size );
    ga->rank = (unsigned int*) calloc( sizeof(unsigned int), ga->population_size );

    // initialize selection size be be 25% of population, minimum of 2
    ga->selection_size = ( ga->population_size >> 2 ) < 2 ? 2 : ga->population_size >> 2;

    // create optimum chromosome (clone)
    ga->c = chromosome_create_clone(_parent);
    ga->utility_opt = ga->minimize ? FLT_MAX : -FLT_MAX;

    //printf("num_parameters: %d\n", ga->num_parameters);
    //printf("population_size: %d\n", ga->population_size);
    //printf("num_mutations: %d\n", ga->num_mutations);
    //printf("\nbits_per_chromosome: %d\n", ga->bits_per_chromosome);

    // initialize population
    unsigned int i;
    // TODO : initialize first chromosome on input vector
    for (i=0; i<ga->population_size; i++) {
        ga->population[i] = chromosome_create_clone(_parent);
        chromosome_init_random( ga->population[i] );
        ga->utility[i] = ga_search_evaluate_chromosome(ga, ga->population[i]);
    }

    // rank chromosomes
    ga_search_rank(ga);

    return ga;
}

void ga_search_destroy(ga_search _g)
{
    unsigned int i;
    for (i=0; i<_g->population_size; i++)
        chromosome_destroy( _g->population[i] );
    free(_g->population);

    // destroy optimum chromosome
    chromosome_destroy(_g->c);

    free(_g->utility);
    free(_g->rank);
    free(_g); // TODO : fix double-free bug
}

void ga_search_print(ga_search _g)
{
    printf("population:\n");
    unsigned int i;
    unsigned int k; // rank[i]
    for (i=0; i<_g->population_size; i++) {
        k = _g->rank[i];
        printf("%4u: [%8.4f] ", k, _g->utility[k]);
        chromosome_printf( _g->population[k] );
    }
}

float ga_search_run(ga_search _g, unsigned int _max_iterations, float _target_utility)
{
    unsigned int i=0;
    do {
        i++;
        ga_search_evolve(_g);
    } while (
        optim_threshold_switch(_g->utility[_g->rank[0]], _target_utility, _g->minimize) &&
        i < _max_iterations);

    // store optimum vector in buffer
    //memmove(_g->v, _g->v_opt, (size_t) (sizeof(float)*_g->num_parameters) );
    return _g->utility_opt;
}

void ga_search_evolve(ga_search _g)
{
    unsigned int i;

    // Crossover
    ga_search_crossover(_g);

    // Mutation
    ga_search_mutate(_g);

    // Evaluate fitness
    for (i=_g->selection_size; i<_g->population_size; i++)
        _g->utility[i] = ga_search_evaluate_chromosome( _g, _g->population[i] );

    // Rank
    ga_search_rank(_g);

    if ( optim_threshold_switch(_g->utility_opt,
                                _g->utility[_g->rank[0]],
                                _g->minimize) )
    {
        // update optimum
        _g->utility_opt = _g->utility[_g->rank[0]];

        // copy optimum chromosome
        chromosome_copy(_g->population[_g->rank[0]], _g->c);
        //memmove(_g->v_opt, _g->v, sizeof(float)*_g->num_parameters);

#if LIQUID_DEBUG_GA_SEARCH
        printf("  utility: %0.2E", _g->utility_opt);
        chromosome_printf(_g->c);
#endif
    }
    
    // set optimum vector to output
    //memmove(_g->v, _g->v_opt, sizeof(float)*_g->num_parameters);
}

void ga_search_getopt(ga_search _g,
                      chromosome _c,
                      float * _optimum_utility)
{
    // copy optimum chromosome
    chromosome_copy(_g->c, _c);

    // copy optimum utility
    *_optimum_utility = _g->utility_opt;
}

float ga_search_evaluate_chromosome(ga_search _g, chromosome _c)
{
    return _g->get_utility(_g->userdata, _c);
}

void ga_search_crossover(ga_search _g)
{
    unsigned int p1, p2;    // index of parents
    unsigned int c;         // index of child
    unsigned int threshold;

    unsigned int i;
    for (i=_g->selection_size; i<_g->population_size; i++) {
        // ensure fittest member is used at least once as parent
        p1 = (i==_g->selection_size) ? _g->rank[0] : _g->rank[rand() % _g->selection_size];
        p2 = _g->rank[rand() % _g->selection_size];
        threshold = rand() % _g->bits_per_chromosome;

        c = _g->rank[i];

        //printf("  ga_search_crossover, p1: %d, p2: %d, c: %d\n", p1, p2, c);
        chromosome_crossover(_g->population[p1],
                             _g->population[p2],
                             _g->population[c],
                             threshold);
    }
}

void ga_search_mutate(ga_search _g)
{
    unsigned int i, j;
    unsigned int index;
    for (i=0; i<_g->population_size; i++) {
        for (j=0; j<_g->num_mutations; j++) {
            index = rand() % _g->bits_per_chromosome;
            chromosome_mutate( _g->population[i], index );
        }
    }
}

void sort_by_index(float *_v, unsigned int* _rank, unsigned int _len, int _descending)
{
    unsigned int i, j, tmp_index;

    for (i=0; i<_len; i++)
        _rank[i] = i;

    for (i=0; i<_len; i++) {
        for (j=_len-1; j>i; j--) {
            //if (_v[_rank[j]]>_v[_rank[j-1]]) {
            if ( optim_threshold_switch(_v[_rank[j]], _v[_rank[j-1]], _descending) ) {
                // swap elements
                tmp_index = _rank[j];
                _rank[j] = _rank[j-1];
                _rank[j-1] = tmp_index;
            }
        }
    }
}

void ga_search_rank(ga_search _g)
{
    sort_by_index( _g->utility, _g->rank, _g->population_size, !(_g->minimize) );
}

