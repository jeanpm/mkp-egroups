/*
 * File:   mkp.h
 * Author: Jean Paulo Martins (jean@icmc.usp.br)
 *
 * Created on 11 de Maio de 2012, 15:25
 * Modified on April 09, 2018
 */

#include <vector>
#include <cstdlib>
#include <cstdio>

#ifndef KNAPSACK_H
#define	KNAPSACK_H

/**
 * Structure representing an instance of the Multidimensional Knapsack Problem.
 */
struct MKP
{
    const char*   instance; // Instance file
    int     nitems;       // Number of items available to be chosen
    int     ndimensions;  // Number of dimensions of each item
    float   bestFeasible; // Known feasible solution fitness value, 0 otherwise
    float   lpOptimal;    // Known Linear relaxation optimal value
    float*  lpsol;
    float** dimensions; // Dimensions of each item (nitems X ndimensions)
    float*  bounds;     // Bounds for each knapsack dimension
    float*  profits;    // Profit of each item
    int*    sortedVars; // Variables sorted in increasing utility-ratio order
    float*  efficiency;
    float*  resources; // Store the occupied resources in each dimension (each constraint)
    int     breakItem; // The index of the critical item
    long    nevals;    // Amount of objective function evaluations
    float   bestFound; // Objective value of the best solution evaluated so far
    long    hittime;   // Number of evaluations when the best was found
};

/*
 * Parameter defining which efficiency measure to use
 */
enum class EfficiencyOrdering
{
    LP_DUALS,
    WEIGHTS_SUM,
    WEIGHTS_SUM_SCALED,
    SENJU_TOYODA,
    FREVILLE_PLATEAU
};

/*
 * Constraint handling by the repair operator of Chu & Beasley (1998) or by the
 * penalty function of Gottlieb (2001).
 */
enum class ConstraintHandling
{
    PENALTY_FUNCTION,
    REPAIR_OPERATOR
};

/*
 * The parameters which define the fitness function to be used.
 */
struct MKPconf
{
    ConstraintHandling chType;
    EfficiencyOrdering eoType;
};


/**
 * Read a MKP instance and initialize the fitness function using the parameters
 * defined by conf.
 *
 * @param file  A file name containing a instance of the MKP
 * @return      A MKP structure
 */
void mkp_readInstance(MKP* mkp, MKPconf* conf, const char* file);

void mkp_lpsolution(MKP* mkp);

double mkp_greedyval(MKP* mkp);
double mkp_igreedyval(MKP* mkp);

void mkp_select_efficiency(MKP* mkp, int choice);

/**
 * The simplest efficiency measure, proportional to the sum of the dimensions
 * of the item j:
 *      e_j = p_j * \sum_{i=1}^{m} w_{ij}
 *
 * @param mkp
 */
void mkp_efficiency_sum(MKP* mkp);


/**
 * The simplest efficiency measure does not take car of the different magnitudes,
 * therefore, a single constraint may dominate others. This drawback is avoided
 * by scaling:
 *      e_j = p_j * \sum_{i=1}^{m} (w_{ij}/c_i)
 *
 * @param mkp
 */
void mkp_efficiency_sum_scaled(MKP* mkp);


/**
 * Taking into account the relative contribution of the constraints. Senju and
 * Toyoda proposed:
 *
 *      e_j = p_j/ \sum_{i=1}^{m} w_{ij} * (\sum_{j=1}^n w_{ij} -c_i)
 *
 * S. Senju and Y. Toyoda. An approach to linear programming with 0–1 variables.
 * Management Science, 15:196–207, 1968.
 */
void mkp_efficiency_Senju_and_Toyoda(MKP* mkp);


/**
 * Taking into account the relative contribution of the constraints. Frévile and
 * Plateau proposed relevance values:
 *
 *      r_i = (\sum_{j=1}^n w_{ij} - c_i)/(\sum_{j=1}^n w_{ij})
 *
 * To be used as:
 *
 *      e_j = p_j/ \sum_{i=1}^{m} w_{ij} * r_i
 *
 * A. Freville and G. Plateau. An efficient preprocessing procedure for the
 * multi-dimensional 0–1 knapsack problem.
 * Discrete Applied Mathematics, 49:189–212, 1994.
 */
void mkp_efficiency_Freville_and_Plateau(MKP* mkp);


/**
 * Taking into account the relative contribution of the constraints and setting
 * them as the values of the dual solution for the relaxed linear problem.
 *
 *      r_i = \lambda_i:
 *
 * To be used as:
 *
 *      p_j/ \sum_{i=1}^{m} r_i * w_{ij}
 *
 * P. C. Chu and J. Beasley. A genetic algorithm for the multiconstrained
 * knapsack problem. Journal of Heuristics, 4:63–86, 1998
 */
void mkp_efficiency_dual(MKP* mkp);


/**
 * Computes the fitness value of a solution. According to MKPconf this fitness
 * can be computed in different ways: repair-operator, penalty-function.
 *
 * @param sol   A MKP candidate solution
 * @param mkp   A MKP instance
 * @param conf  Configuration to fitness computation: constraint handling
 * @return      The fitness value
 */
double mkp_fitness (int* sol, MKP* mkp, MKPconf* conf);

double mkp_penaltyFitness(int* sol, MKP* mkp );

double mkp_repairFitness(int* sol, MKP* mkp);


int  mkp_getBreakItem (MKP* mkp);

/**
 * Try to add an item in the knapsack, if it does not fit, the solution is not
 * modified and false is returned. This function also updates the resource
 * consumption vector.
 *
 * @param sol
 * @param item
 * @param mkp
 * @return
 */
bool mkp_addItem (int* sol, int item, MKP* mkp);

/**
 * Removes an item from the knapsack, if after remotion the solution becomes
 * feasible true is returned. This function also updates the resource consumption
 * vector.
 *
 * @param sol
 * @param item
 * @param mkp
 * @return
 */
bool mkp_removeItem (int* sol, int item, MKP* mkp);

/**
 * Write a MKP instance to a file.
 *
 * @param mkp - A MKP instance
 * @param out - An opened file to be written
 */
void mkp_writeInstance(MKP* mkp, FILE* out);


/**
 * Returns true if the solution is feasible and false otherwise, according
 * instance mkp.
 *
 * @param sol
 * @param mkp
 * @return
 */
bool mkp_isFeasible(int* sol, MKP* mkp);


/**
 * Repair an unfeasible solution using the DROP-ADD method described by
 * Chu and Beasley (1998).
 *
 * @param sol
 * @param mkp
 */

void mkp_repairUnfeasibleSolution(int* sol, MKP* mkp);

/**
 * Free the memory used by an instance.
 *
 * @param inst - Instance to be removed from memory
 */
void mkp_free(MKP* inst);


#endif	/* KNAPSACK_H */
