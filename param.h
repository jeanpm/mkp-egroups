#ifndef PARAM_H
#define PARAM_H

#include "mkp.h"
#include "egroups.h"

#include <vector>
#include <getopt.h>
#include <functional>


using namespace std;

struct parameters
{
    char* instance; // MKP instance file
    int popsize; // Population size
    int nitems;  // Number of items in the MKP instance

    GroupMutation mutation_type; // Type of group mutation
    string mutation_type_str;
    int decimal_cases; // Number of decimal cases to identify efficiency groups: 1,2
    int mutation_treshold; // Minimum number of improvements to avoid group mutation

    float bias_towards_LP;  // Initial bias towards an LP-optimal solution
    float learning_rate;   // Learning rate to update the univariate model

    long maxevals;

    std::function<void(std::vector<egroup>&, MKP*)> group_mutation;

    parameters(int argc, char** argv);

    void print_usage();
    void print_param();
};


#endif
