#include "param.h"
#include <iostream>

parameters::parameters(int argc, char** argv)
{
    // Default values
    instance = NULL;
    popsize  = 100;
    mutation_type = (GroupMutation)3;
    decimal_cases = 1;
    mutation_treshold = 0;
    bias_towards_LP = 0;
    learning_rate  = 0.001;
    maxevals = 1e5;
    group_mutation = EG_donothing;

    // Parse the parameters using getopt.h
    char c;
    while ((c = getopt (argc, argv, ":i:n:m:d:t:b:l:e:")) != -1)
    {
        switch (c) {
            case 'i': instance = optarg;        break;
            case 'n': popsize = atoi(optarg);   break;
            case 'm': mutation_type = (GroupMutation)atoi(optarg); break;
            case 'd': decimal_cases = atoi(optarg);         break;
            case 't': mutation_treshold = atoi(optarg);     break;
            case 'b': bias_towards_LP = atof(optarg);       break;
            case 'l': learning_rate  = atof(optarg);        break;
            case 'e': maxevals = atol(optarg);              break;
            case '?': print_usage();                        exit(-1);
        }
    }

    switch (mutation_type) {
        case GroupMutation::RANDOM_SWAP:
            mutation_type_str = "Random group swap (CBGA_rg_swap)";
            group_mutation = EG_random_group_swap;
            break;
        case GroupMutation::RANDOM_SHUFFLE:
            mutation_type_str = "Random group shuffle (CBGA_rg_shuffle)";
            group_mutation = EG_random_group_shuffle;
            break;
        case GroupMutation::SHUFFLE_ALL:
            mutation_type_str = "Random shuffle all groups (CBGA_all_shuffle)";
            group_mutation = EG_shuffle_all;
            break;
        default:
            mutation_type_str = "No group mutation (CBGA)";
            group_mutation = EG_donothing;
            break;
    }

#ifdef DEBUG
    print_param();
#endif
}

void parameters::print_usage()
{
    cerr << endl << "Usage information:" << endl
        << "\t-i string \t" << "MKP instance file" << endl
        << "\t-n integer\t"  << "population size"   << endl
        << "\t-m option \t"  << "mutation type: 0,1,2,3"   << endl
        << "\t-d integer\t"  << "decimal cases for rounding: 1,2"   << endl
        << "\t-t integer\t"  << "mutation threshold"   << endl
        << "\t-b float  \t"  << "bias towards LP solution"   << endl
        << "\t-l float  \t"  << "learning rate"   << endl
        << "\t-e long   \t"  << "max evaluations"   << endl << endl;
}

void parameters::print_param()
{
    cerr << endl << "Current settings:" << endl
        << "\t-i instance \t" << instance << endl
        << "\t-n popsize  \t"  << popsize   << endl
        << "\t-m mutation \t"  << mutation_type_str   << endl
        << "\t-d dcases   \t"  << decimal_cases   << endl
        << "\t-t threshold\t"  << mutation_treshold   << endl
        << "\t-b LP bias  \t"  << bias_towards_LP   << endl
        << "\t-l learning \t"  << learning_rate   << endl
        << "\t-e maxeval  \t"  << maxevals   << endl << endl;
}
