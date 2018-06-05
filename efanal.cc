#include "mkp.h"

#include <cassert>
#include <random>
#include <iostream>
#include <queue>
#include <list>
#include <algorithm>
#include <chrono>

using namespace std;

/* Round a floating point number to a number of decimal cases */
double round_to(double value, int dcases)
{
	double power = pow(10, dcases);
	value = round( value * power)/power;
	return value;
}

struct group {
    int a;
    int b;
    int size() { return b - a;}
};

/*
 * A group is a subset of items whose efficiencies are equal given a precision
 * 'dcases'. For example: 1.4356 and 14389 are equal if dcases==2.
 */
void mkp_getEfficiencyGroups(vector<group>& groups, MKP* m, int dcases)
{
	int* I = m->sortedVars;
	float* ef = m->efficiency;

	// defines an interval by indexes
	int a = 0;
	for (int i = 1; i < m->nitems; i++)
	{
		if (round_to(ef[I[i]], dcases) == round_to(ef[I[a]], dcases))
			continue;
		else if (a == i - 1) { // previous interval is empty, starts a new one
			a = i;
		} else { // close the interval
			groups.push_back(group{a, i});
			a = i;
		}
	}
}

int main(int argc, char** argv)
{
	MKPconf conf;
	conf.chType = ConstraintHandling::REPAIR_OPERATOR;
	conf.eoType = EfficiencyOrdering::LP_DUALS;

	// Reading instance data
	MKP* mkp = new MKP;
	mkp_readInstance(mkp, &conf, argv[1]);
	
	vector<group> egroups;
	
	for (int i = 0; i < 8; i++)
	{
	    mkp_getEfficiencyGroups(egroups, mkp, i);
	    printf("%d: ", i);
	    for (group g : egroups) printf("%d ", g.size());
	    printf("\n");
	    egroups.clear();
	}


//	for (int i = 0; i < mkp->nitems; i++)
//		printf("%lf ", mkp->efficiency[mkp->sortedVars[i]]);
//	printf("\n");

	mkp_free(mkp);
}
