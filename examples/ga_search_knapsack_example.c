//
// ga_search_knapsack_example.c
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "ga_search_knapsack_example.m"

// knapsack item
struct knapsack_item_s {
    float weight;
    float value;
};

// knapsack
struct knapsack_s {
    struct knapsack_item_s * items;
    unsigned int num_items;
    float capacity;
};

typedef struct knapsack_s * knapsack;

knapsack knapsack_create(float _capacity,
                         struct knapsack_item_s * _items,
                         unsigned int _num_items);
void knapsack_destroy(knapsack _q);
void knapsack_print(knapsack _q);
float knapsack_utility(void * _userdata, chromosome _c);

int main() {
    unsigned int num_items = 1000;      // number of items in the knapsack
    unsigned int num_iterations = 8000; // number of iterations to run
    float capacity = 20.0f;             // total capacity of the knapsack
    unsigned int population_size = 100; // number of chromosomes in the population
    float mutation_rate = 0.40f;        // mutation rate of the GA

    unsigned int i;

    // create items (random weight, value)
    struct knapsack_item_s items[num_items];
    for (i=0; i<num_items; i++) {
        items[i].weight = randf();
        items[i].value  = randf();
    }

    // create knapsack
    knapsack bag = knapsack_create(capacity,
                                   items,
                                   num_items);
    knapsack_print(bag);

    // create prototype chromosome (1 bit/item)
    chromosome prototype = chromosome_create_basic(num_items, 1);
    chromosome_print(prototype);

    float optimum_utility;

    // open output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");

    // create ga_search object
    ga_search ga = ga_search_create_advanced(&knapsack_utility,
                                             (void*)bag,
                                             prototype,
                                             LIQUID_OPTIM_MAXIMIZE,
                                             population_size,
                                             mutation_rate);

    // execute search
    //optimum_utility = ga_search_run(ga, num_iterations, -1e-6f);

    // execute search one iteration at a time
    fprintf(fid,"u = zeros(1,%u);\n", num_iterations);
    for (i=0; i<num_iterations; i++) {
        ga_search_evolve(ga);

        ga_search_getopt(ga, prototype, &optimum_utility);
        if (((i+1)%100)==0)
            printf("  %4u : %12.8f;\n", i+1, optimum_utility);

        fprintf(fid,"u(%3u) = %12.4e;\n", i+1, optimum_utility);
    }

    // print results
    printf("\n");

    ga_search_getopt(ga, prototype, &optimum_utility);
    float total_value = 0.0f;
    float total_weight = 0.0f;
    for (i=0; i<num_items; i++) {
        if ( chromosome_value(prototype,i) == 1 ) {
            // include this item into knapsack
            total_value  += items[i].value;
            total_weight += items[i].weight;
        }
        printf("  %3u : %8.4f @ $%8.4f %c\n", i, items[i].weight, items[i].value, chromosome_value(prototype,i) ? '*' : ' ');
    }


    fprintf(fid,"figure;\n");
    //fprintf(fid,"semilogy(u);\n");
    fprintf(fid,"plot(u);\n");
    fprintf(fid,"xlabel('iteration');\n");
    fprintf(fid,"ylabel('utility');\n");
    fprintf(fid,"title('GA search results');\n");
    fprintf(fid,"grid on;\n");
    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    // test results, optimum at [1, 1, 1, ... 1];

    chromosome_destroy(prototype);
    ga_search_destroy(ga);
    knapsack_destroy(bag);

    return 0;
}


// 
// knapsack methods
//

// create knapsack object
knapsack knapsack_create(float _capacity,
                         struct knapsack_item_s * _items,
                         unsigned int _num_items)
{
    knapsack q = (knapsack) malloc(sizeof(struct knapsack_s));
    q->capacity = _capacity;
    q->items = _items;
    q->num_items = _num_items;
    return q;
}

void knapsack_destroy(knapsack _q)
{
    free(_q);
}

void knapsack_print(knapsack _q)
{
    unsigned int i;
    printf("knapsack: %u items, capacity : %12.8f\n", _q->num_items, _q->capacity);
    for (i=0; i<_q->num_items; i++)
        printf("  %3u : %8.4f @ $%8.4f\n", i, _q->items[i].weight, _q->items[i].value);
}

// compute utility for this chromosome
float knapsack_utility(void * _userdata, chromosome _c)
{
    knapsack _q = (knapsack) _userdata;

    // chromosome represents number of each item in knapsack
    float total_value = 0;
    float total_weight = 0;
    unsigned int i;
    for (i=0; i<_q->num_items; i++) {
        if ( chromosome_value(_c,i) == 1 ) {
            // include this item into knapsack
            total_value  += _q->items[i].value;
            total_weight += _q->items[i].weight;
        }
    }

    // check for invalid solution
    if (total_weight > _q->capacity)
        return _q->capacity - total_weight;

    return total_value;
}

