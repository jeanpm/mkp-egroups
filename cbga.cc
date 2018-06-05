#include "cbga.h"
#include "global.h"

#include <algorithm>

/*
 * Return the index of the winner solution of a binary tournament
 */
int  cb_bintournament (population& pop)
{
    int a = unif(gen) * pop.m;
    int b = unif(gen) * pop.m;
    return (pop.fx[a] > pop.fx[b]) ? a : b;
}

/*
 * Returns a child produced by the uniform crossover between two parents.
 */
void cb_uniformxover (int* parentA, int* parentB, int* offspring, int nvars)
{
    for (int i = 0; i < nvars; ++i)
    {
        if (unif01(gen)) offspring[i] = parentA[i];
        else  offspring[i] = parentB[i];
    }
}

/*
 * Mutate two randomly chosen bits from a solution
 */
void  cb_mutation (int* offspring, int nvars)
{
    int rbit = unif(gen) * nvars;
    offspring[ rbit ] = 1 - offspring[ rbit ];

    rbit = unif(gen) * nvars;
    offspring[ rbit ] = 1 - offspring[ rbit ];
}

/*
 * Generates an offspring as defined by Chu & Beasley (1998)
 */
void cbga_sample(population& pop, int* tmp)
{
    int pa = cb_bintournament(pop);
    int pb = cb_bintournament(pop);

    cb_uniformxover(pop.p[pa], pop.p[pb], tmp, pop.n);
    cb_mutation(tmp, pop.n);
}

/*
 * Sets the opposite solution (pop.otmp) from the last sampled solution (pop.tmp)
 */
void cbga_opposite_sample(population& pop)
{
    for (int i = 0; i < pop.n; i++)
        pop.otmp[i] = 1 - pop.tmp[i];
}

/*
 * A genetic algorithm for the multidimensional knapsack problem (Chu & Beasley,1998)
 */
int cbga(population& pop, model& univ, MKP* mkp)
{
    float new_fx = 0;
    int nimprovements = 0; // It will never stop from stagnation
    for (int i = 0; i < pop.m; i++)
    {
        // Chu&Beasley GA
        cbga_sample(pop, pop.tmp);

        // Repair if infeasible
        new_fx = mkp_repairFitness(pop.tmp, mkp);

        // Adds the the new solution to the population and update frequencies
        if (pop.add_tmp(new_fx, univ))
            nimprovements++;

        #ifdef OBL
            cbga_opposite_sample(pop);
            float ofx = mkp_repairFitness(pop.otmp, mkp);
            // Adds the the new solution to the population and update frequencies
            std::swap(pop.tmp, pop.otmp);
            if (pop.add_tmp(ofx, univ))
                nimprovements++;
        #endif

        if (stop_criterion(mkp))
            return EXIT_STATUS;
    }  

    return nimprovements;
}
