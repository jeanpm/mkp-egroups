#include "mkp.h"
#include "pop.h"
#include "cbga.h"
#include "model.h"
#include "param.h"
#include "global.h"
#include "runinfo.h"
#include "egroups.h"

#include <queue>
#include <random>

#include <cassert>
#include <iostream>
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
    MAXEVALS = p.maxevals;
    return mkp;
}


int main(int argc, char** argv)
{
    parameters p(argc, argv);

	MKP* mkp = init_MKP(p);

	// [beginning]
	runinfo s;
    s.begin();

	vector<egroup> egroups;
    EG_getEfficiencyGroups(egroups, mkp, p.decimal_cases);

	// Initializes an univariate model
	model univ(p.learning_rate, p.nitems);
	univ.bias_towards(mkp->lpsol, p.bias_towards_LP, false);

	population pop(p.popsize, p.nitems, univ);
	pop.evaluate(mkp);

	// Resample new solutions until the bestknown is found or MAXEVALS reached
	int nimprovements = 0;
	do {
        nimprovements = cbga(pop, univ, mkp);
        if (nimprovements <= p.mutation_treshold)
            p.group_mutation(egroups, mkp);
	} while (!finished());
	// [ending]

	s.end(pop, univ, mkp);
	s.print();

	mkp_free(mkp);
}
