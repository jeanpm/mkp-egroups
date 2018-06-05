#include "mkp.h"

#include <stdlib.h>
#include <time.h>
#include <list>
#include <cstdio>
#include <vector>
#include <math.h>
#include <random>
#include <errno.h>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <string.h>
#include <algorithm>
#include <random>

#include "global.h"


using namespace std;

/**
 * Read a MKP instance.
 *
 * @param file - A file name containing a instance of the MKP
 * @return A MKP structure
 */
void mkp_readInstance(MKP* mkp, MKPconf* mkpconf, const char* file)
{
    srand(time(NULL));

    mkp->instance = file;

    FILE* finstance = fopen(file, "r");
    if (finstance == NULL)
    {
        fprintf(stderr, "Error opening file %s\t[%s]\n", file, strerror(errno));
        exit(0);
    }

    int status = 0;
    status = fscanf(finstance, "%d", &(mkp->nitems));
    status = fscanf(finstance, "%d", &(mkp->ndimensions));
    status = fscanf(finstance, "%f", &(mkp->bestFeasible));
    status = fscanf(finstance, "%f", &(mkp->lpOptimal));

    mkp->profits    = new float [mkp->nitems];
    mkp->bounds     = new float [mkp->ndimensions];
    mkp->dimensions = new float*[mkp->nitems];
    mkp->resources  = new float [mkp->ndimensions];
    mkp->lpsol      = new float [mkp->nitems];

    mkp->sortedVars = new int[mkp->nitems];
    mkp->efficiency = new float[mkp->nitems];

    for (int i = 0; i < mkp->nitems; ++i)
        status = fscanf(finstance, "%f", &(mkp->profits[i]));

    for (int j = 0; j < mkp->nitems; ++j)
        mkp->dimensions[j] = new float[mkp->ndimensions];

    for (int i = 0; i < mkp->ndimensions; ++i)
        for (int j = 0; j < mkp->nitems; ++j)
            status = fscanf(finstance, "%f", &(mkp->dimensions[j][i]));

    for (int i = 0; i < mkp->ndimensions; ++i)
    {
        mkp->resources[i] = 0.0;
        status = fscanf(finstance, "%f", &(mkp->bounds[i]));
    }

    fclose(finstance);

    if (mkpconf->chType == ConstraintHandling::REPAIR_OPERATOR)
    {
        switch (mkpconf->eoType) {
            case EfficiencyOrdering::LP_DUALS :
                mkp_efficiency_dual(mkp);
                break;
            case EfficiencyOrdering::WEIGHTS_SUM :
                mkp_efficiency_sum(mkp);
                break;
            case EfficiencyOrdering::WEIGHTS_SUM_SCALED :
                mkp_efficiency_sum_scaled(mkp);
                break;
            case EfficiencyOrdering::SENJU_TOYODA :
                mkp_efficiency_Senju_and_Toyoda(mkp);
                break;
            case EfficiencyOrdering::FREVILLE_PLATEAU :
                mkp_efficiency_Freville_and_Plateau(mkp);
                break;
            default :
                fprintf(stderr, "Wrong EfficiencyOrdering type!\n");
                exit(1);
        }
        mkp->breakItem = mkp_getBreakItem( mkp );
    }

    mkp->nevals = 0;
    mkp->bestFound = 0.0;
    mkp->hittime = 0;
}

/**
 * Evaluates the fitnesse of a solution according certain instance.
 *
 * @param sol   - A solution
 * @param inst  - A MKP instance
 * @return fitness value.
 */
double mkp_repairFitness(int* sol, MKP* mkp)
{
    mkp_repairUnfeasibleSolution(sol, mkp);

    double fitness = 0.0;
    for (int j = 0; j < mkp->nitems; ++j)
        fitness += sol[j] * mkp->profits[j];

    // Update information about the run
    if (fitness > mkp->bestFound) {
        mkp->bestFound = fitness;
        mkp->hittime = mkp->nevals;
    }
    mkp->nevals++;

    return fitness;
}

/*
 * Gottlieb (2001) Penalty-function, pCOR in the paper
 * "On the Feasibility Problem of Penalty-Based Evolutionary Algorithms
 * for Knapsack Problems"
 *
 */
double mkp_penaltyFitness(int* sol, MKP* mkp )
{
    double fitness = 0.0;
    for (int j = 0; j < mkp->nitems; ++j)
        fitness += sol[j] * mkp->profits[j];

    // Maximum amount of Constraint Violation (Gottlieb, 2001)
    double maxCV = 0.0;
    for (int i = 0; i < mkp->ndimensions; ++i)
    {
        // Amount of constraint violation for constraint i (dimension i)
        double CVi = 0.0;
        for (int j = 0; j < mkp->nitems; ++j)
            CVi += sol[j] * mkp->dimensions[j][i];

        CVi -= mkp->bounds[i];
        CVi = (CVi > 0) ? CVi : 0.0;
        maxCV = (CVi > maxCV) ? CVi : maxCV;
    }

    // Maximum profit coefficient (Gottlieb, 2001)
    double pmax = 0.0;
    for (int j = 0; j < mkp->nitems; ++j)
        pmax = (pmax > mkp->profits[j]) ? pmax : mkp->profits[j];

    // Minimum resource consumption rmin (Gottlieb, 2001)
    double rmin = 9999999;
    for (int i = 0; i < mkp->ndimensions; ++i)
        for (int j = 0; j < mkp->nitems; ++j)
            rmin = (rmin < mkp->dimensions[j][i]) ? rmin : mkp->dimensions[j][i];

    double pCOR = ((pmax + 1)/rmin) * maxCV;

    return fitness - pCOR;
}


/*
 * Return the fitness value, using the repair operator or penalty-function,
 * as defined in the mkpconf structure.
 */
double mkp_fitness (int* sol, MKP* mkp, MKPconf* mkpconf)
{
    if (mkpconf->chType == ConstraintHandling::PENALTY_FUNCTION)
        return mkp_penaltyFitness(sol, mkp);
    if (mkpconf->chType == ConstraintHandling::REPAIR_OPERATOR)
        return mkp_repairFitness(sol, mkp);
    return 0;
}


/*
 * Computes the break item according to the efficiencies ordering defined at
 * the initialization.
 */
int mkp_getBreakItem(MKP* mkp)
{
    int* sol = new int[mkp->nitems];
    memset(sol, 0, sizeof(int) * mkp->nitems);

    int i = 0;
    do {
        sol[mkp->sortedVars[i++]] = 1;
    } while ( mkp_isFeasible(sol, mkp) );

    delete[] sol;

    return i -1;
}


/*
 * Efficiency proportional to the sum of the dimensions of the item j:
 *      e_j = p_j * \sum_{i=1}^{m} w_{ij}
 */
void mkp_efficiency_sum(MKP* mkp)
{
    for (int i = 0; i < mkp->nitems; ++i)
    {
        double weights = 0;
        for (int j = 0; j < mkp->ndimensions; ++j)
            weights += double(mkp->dimensions[i][j]);//mkp->bounds[j];
        mkp->efficiency[i] = double(mkp->profits[i])/weights;
        mkp->sortedVars[i] = i;
    }

    sort(mkp->sortedVars, mkp->sortedVars + mkp->nitems
        , [mkp] (int i, int j) {return (mkp->efficiency[i] > mkp->efficiency[j]);} );
}


/*
 * Efficiency proportional to the sum of the dimensions of the item j scaled
 *      e_j = p_j * \sum_{i=1}^{m} (w_{ij}/c_i)
 */
void mkp_efficiency_sum_scaled(MKP* mkp)
{
    for (int i = 0; i < mkp->nitems; ++i)
    {
        double weights = 0;
        for (int j = 0; j < mkp->ndimensions; ++j)
            weights += double(mkp->dimensions[i][j])/mkp->bounds[j];

        mkp->efficiency[i] = double(mkp->profits[i])/weights;
        mkp->sortedVars[i] = i;
    }

    sort(mkp->sortedVars, mkp->sortedVars + mkp->nitems
        , [mkp] (int i, int j) {return (mkp->efficiency[i] > mkp->efficiency[j]);} );
}


/**
 * S. Senju and Y. Toyoda. efficiencies
 *      e_j = p_j/ \sum_{i=1}^{m} w_{ij} * (\sum_{j=1}^n w_{ij} -c_i)
 */
void mkp_efficiency_Senju_and_Toyoda(MKP* mkp)
{
    double* w = new double[mkp->ndimensions];
    for (int j = 0; j < mkp->ndimensions; ++j)
    {
        w[j] = 0.0;
        for (int i = 0; i < mkp->nitems; ++i)
            w[j] += mkp->dimensions[i][j];
        w[j] -=  mkp->bounds[j];
    }


    for (int i = 0; i < mkp->nitems; ++i)
    {
        double weights = 0;
        for (int j = 0; j < mkp->ndimensions; ++j)
            weights += double(mkp->dimensions[i][j]) * w[j];

        mkp->efficiency[i] = double(mkp->profits[i])/weights;
        mkp->sortedVars[i] = i;
    }

    sort(mkp->sortedVars, mkp->sortedVars + mkp->nitems
        , [mkp] (int i, int j) {return (mkp->efficiency[i] > mkp->efficiency[j]);} );
    delete[] w;
}


/*
 * Frévile and Plateau efficiencies
 *      r_i = (\sum_{j=1}^n w_{ij} - c_i)/(\sum_{j=1}^n w_{ij})
 *      e_j = p_j/ \sum_{i=1}^{m} w_{ij} * r_i
 */
void mkp_efficiency_Freville_and_Plateau(MKP* mkp)
{
    double* w = new double[mkp->ndimensions];
    for (int j = 0; j < mkp->ndimensions; ++j)
    {
        w[j] = 0.0;
        for (int i = 0; i < mkp->nitems; ++i)
            w[j] += mkp->dimensions[i][j];
        w[j] =  (w[j]-mkp->bounds[j])/w[j];
    }

    for (int i = 0; i < mkp->nitems; ++i)
    {
        double weights = 0;
        for (int j = 0; j < mkp->ndimensions; ++j)
            weights += double(mkp->dimensions[i][j]) * w[j];

        mkp->efficiency[i] = double(mkp->profits[i])/weights;
        mkp->sortedVars[i] = i;
    }

    sort(mkp->sortedVars, mkp->sortedVars + mkp->nitems
        , [mkp] (int i, int j) {return (mkp->efficiency[i] > mkp->efficiency[j]);} );

    delete[] w;
}

void mkp_lpsolution(MKP* mkp)
{
    string lpfile (mkp->instance);
    lpfile = lpfile + ".lp";

    FILE* file = fopen(lpfile.c_str(), "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error opening file %s\t[%s]\n"
                , lpfile.c_str(), strerror(errno));
        exit(0);
    }

    for (int i = 0; i < mkp->nitems; ++i)
        int status = fscanf(file, "%f", &(mkp->lpsol[i]));

    fclose(file);
}

/**
 * Dual efficiencies
 *      r_i = \lambda_i
 *      p_j/ \sum_{i=1}^{m} r_i * w_{ij}
 */
void mkp_efficiency_dual(MKP* mkp)
{
    string dualfile (mkp->instance);
    dualfile = dualfile + ".dual";

    FILE* file = fopen(dualfile.c_str(), "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error opening file %s\t[%s]\n"
                , dualfile.c_str(), strerror(errno));
        exit(0);
    }

    double* w = new double[mkp->ndimensions];
    for (int i = 0; i < mkp->ndimensions; ++i)
        int status = fscanf(file, "%lf", &w[i]);

    fclose(file);

    for (int i = 0; i < mkp->nitems; ++i)
    {
        double weights = 0;
        for (int j = 0; j < mkp->ndimensions; ++j)
            weights += double(mkp->dimensions[i][j]) * w[j];

        mkp->efficiency[i] = double(mkp->profits[i])/weights;
        mkp->sortedVars[i] = i;
    }

    sort(mkp->sortedVars, mkp->sortedVars + mkp->nitems
        , [mkp] (int i, int j) {return (mkp->efficiency[i] > mkp->efficiency[j]);} );

    delete[] w;
}


/*
 * Verifies if a solution is feasible, also fills the resource vector
 */
bool mkp_isFeasible(int* sol, MKP* mkp)
{
    bool isFeasible = true;
    for (int i = 0; i < mkp->ndimensions; ++i)
    {
        mkp->resources[i] = 0.0;
        // Counts the space occupied by all items in dimension i
        for (int j = 0; j < mkp->nitems; ++j)
            mkp->resources[i] += sol[j] * mkp->dimensions[j][i];

        if (mkp->resources[i] > mkp->bounds[i])
            isFeasible = false;
    }

    return isFeasible;
}

/*
 * Remove an item and update the resources vector
 */
bool mkp_removeItem(int* sol, int item, MKP* mkp)
{
    // Remove item
    sol[item] = 0;

    // Update resources vector
    bool isFeasible = true;
    for (int i = 0; i < mkp->ndimensions; ++i)
    {
        mkp->resources[i] -= mkp->dimensions[item][i];
        if (mkp->resources[i] > mkp->bounds[i])
            isFeasible = false;
    }
    return isFeasible;
}


/*
 * Add an item to the knapsack and update the resources vector
 */
bool mkp_addItem(int* sol, int item, MKP* mkp)
{
    // Verifies if item fits
    for (int i = 0; i < mkp->ndimensions; ++i)
        if (mkp->resources[i] + mkp->dimensions[item][i] > mkp->bounds[i])
            return false;

    // "If I fits I sits :P"
    // Add item and Update resources vector
    sol[item] = 1;
    for (int i = 0; i < mkp->ndimensions; ++i)
        mkp->resources[i] += mkp->dimensions[item][i];

    return true;
}


void mkp_repairUnfeasibleSolution(int* sol, MKP* mkp)
{
    if (mkp_isFeasible(sol, mkp)) return;

    /*
     * Remove the items with worst utility ratio until a feasible solution is
     * found. DROP step (Chu and Beasley, 1998)
     */
    for (int i = mkp->nitems-1; i >= 0; --i)
    {
        if (sol[mkp->sortedVars[i]] == 1 )
            if (mkp_removeItem(sol, mkp->sortedVars[i], mkp)) break;
    }

    /*
     * Try to add the items with best utility ratios which keeps the feasibility
     * of the solution. Each inserted item which makes the solution infeasible
     * is removed and the next is tried. ADD step (Chu and Beasley, 1998)
     */
    for (int i = 0; i < mkp->nitems; ++i)
    {
        if (sol[mkp->sortedVars[i]] == 0 )
            mkp_addItem(sol, mkp->sortedVars[i], mkp);
    }
}

/*
 * Write a MKP instance to a file.
 */
void mkp_writeInstance(MKP* mkp, FILE* out)
{
    fprintf(out, "%d " , mkp->nitems);
    fprintf(out, "%d " , mkp->ndimensions);
    fprintf(out, "%.2f\n", mkp->bestFeasible);
    fprintf(out, "%.2f\n", mkp->lpOptimal);

    for (int i = 0; i < mkp->nitems; ++i)
        fprintf(out, "%.1f ", mkp->profits[i]);
    fprintf(out, "\n");

    for (int i = 0; i < mkp->ndimensions; ++i)
        for (int j = 0; j < mkp->nitems; ++j)
            fprintf(out, "%.1f ", mkp->dimensions[j][i]);
    fprintf(out, "\n");

    for (int i = 0; i < mkp->ndimensions; ++i)
        fprintf(out, "%.1f ", mkp->bounds[i]);
    fprintf(out, "\n");
}


/**
 * Free the memory used by an instance.
 *
 * @param inst - Instance to be removed from memory
 */
void mkp_free(MKP* mkp)
{
    delete[] mkp->profits;
    delete[] mkp->bounds;

    for (int i = 0; i < mkp->nitems; ++i)
        delete[] mkp->dimensions[i];

    delete[] mkp->dimensions;
    delete[] mkp->resources;

    delete[] mkp->sortedVars;
    delete[] mkp->efficiency;

    delete mkp;
}
