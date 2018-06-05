#ifndef CBGA_H
#define CBGA_H

#include "pop.h"
#include "model.h"
#include "mkp.h"

/*
 * Return the index of the winner solution of a binary tournament
 */
int  cb_bintournament (population& pop);

/*
 * Returns a child produced by the uniform crossover between two parents.
 */
void cb_uniformxover (int* parentA, int* parentB, int* offspring, int nvars);

/*
 * Mutate two randomly chosen bits from a solution
 */
void  cb_mutation (int* offspring, int nvars);

/*
 * Generates an offspring as defined by Chu & Beasley (1998)
 */
void cbga_sample(population& pop, int* tmp);

/*
 * A genetic algorithm for the multidimensional knapsack problem (Chu & Beasley,1998)
 */
int cbga(population& pop, model& univ, MKP* mkp);

#endif
