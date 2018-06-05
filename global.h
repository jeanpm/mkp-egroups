#ifndef GLOBAL_H
#define GLOBAL_H

#include <random>
#include "mkp.h"

static std::random_device device;
static const long seed = device();
static std::mt19937 gen(seed);
static std::bernoulli_distribution unif01(0.5);
static std::uniform_real_distribution<double> unif(0.0,1.0);

extern long MAXEVALS;
extern int EXIT_STATUS;

#define MAXEVALS_REACHED -99
#define BESTKNOWN_FOUND  -98
#define CONTINUE_SEARCH  -97

/*
 The search for better solutions must stop if the maximum of objective function
 evaluations was reached OR if the best solution found so far is already better
 than the best solution known in the literature.
 */
bool stop_criterion(MKP* m);

/*
 Utility function that returns true if the search should stop
 */
bool finished();

#endif
