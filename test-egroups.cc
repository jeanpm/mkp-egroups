#include "mkp.h"
#include "cbga.h"
#include "param.h"
#include "global.h"
#include "egroups.h"

#include <list>
#include <queue>
#include <random>
#include <chrono>
#include <cassert>
#include <iostream>
#include <cstring>
#include <algorithm>

using namespace std;



MKP* init_MKP(parameters& p)
{
    MKPconf conf;
	conf.chType = ConstraintHandling::REPAIR_OPERATOR;
	conf.eoType = EfficiencyOrdering::LP_DUALS;

	// Reading instance data
	MKP* mkp = new MKP;
	mkp_readInstance(mkp, &conf, p.instance);
	mkp_lpsolution(mkp);
    p.nitems = mkp->nitems;
    return mkp;
}

double greedy(MKP* mkp)
{   
    int sol[mkp->nitems] = {0};

    int i = 0;
    do {
        sol[mkp->sortedVars[i++]] = 1;
    } while ( mkp_isFeasible(sol, mkp) );

    sol[i-1] = 0;
    return mkp_repairFitness(sol, mkp);
}


int main(int argc, char** argv)
{
    parameters p(argc, argv);
    
	MKP* mkp = init_MKP(p);
	
	vector<egroup> egroups = EG_getEfficiencyGroups(mkp, p.decimal_cases);
    
    int i = 0;
    
    int* bestOrd = new int[mkp->nitems];
    copy(mkp->sortedVars, mkp->sortedVars + mkp->nitems, bestOrd);
    
    double bestfx = greedy(mkp);
    
    printf("From %lf -> ", bestfx);
    while (i++ < 10000) 
    {
        double fx = greedy(mkp);
        if (fx > bestfx) {
            bestfx = fx;
            copy(mkp->sortedVars, mkp->sortedVars + mkp->nitems, bestOrd);
            printf("%lf\n", fx);
        } else if (fx < bestfx ) {
            copy(bestOrd, bestOrd + mkp->nitems, mkp->sortedVars);
        }

	    p.group_mutation(egroups, mkp);
	}
    printf("%lf, Best known is %lf\n", bestfx, mkp->bestFeasible);


	mkp_free(mkp);
}
