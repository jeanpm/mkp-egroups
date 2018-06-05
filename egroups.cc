#include "egroups.h"
#include "global.h"

#include <random>
#include <cassert>
#include <stack>

using namespace std;

/*
 Round a double to a certain number of decimal cases
 */
double round_to(double value, int dcases)
{
	double power = pow(10, dcases);
	value = round(value * power)/power;
	return value;
}


/*
 An efficiency group is defined by items whose efficiencies are equal.
 If efficiencies are rounded down, the groups are likely to be larger.
 */
void EG_getEfficiencyGroups(vector<egroup>& groups, MKP* m, int dcases)
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
			groups.push_back(egroup{a, i});
			a = i;
		}
	}
}

void EG_donothing(std::vector<egroup>& egroups, MKP* m)
{}

/*
 Shuffle the ordering of intra-group items, for all groups.
 */
void EG_shuffle_all(vector<egroup>& egroups, MKP* m)
{
    assert(egroups.size() > 0);

	uniform_int_distribution<int> rint(0, m->nitems-1);
    // Just an alias
	int* I = m->sortedVars;

	for (int i = 0; i < egroups.size(); i++) {
        egroup gi = egroups[i];
		for (int j = gi.a; j < gi.b; j++) {
            // swap the order of two items within a group
			int r = rint(gen) % (gi.b - j);
			std::swap(I[j], I[j + r]);
		}
	}
}

/*
 Shuffle the ordering of intra-group items, for a random group
 */
void EG_random_group_shuffle(vector<egroup>& egroups, MKP* m)
{
    assert(egroups.size() > 0);

    uniform_int_distribution<int> relem(0, m->nitems-1);
	uniform_int_distribution<int> rint(0, egroups.size()-1);
    // Just an alias
	int* I = m->sortedVars;

    egroup gi = egroups[rint(gen)];

    std::stack<egroup> bt;

	for (int j = gi.a; j < gi.b; j++) {
        // swap the order of two items within a group
		int r = relem(gen) % (gi.b - j);
		std::swap(I[j], I[j + r]);
		bt.push(egroup{j, j+r});
	}
}

/*
 Swap the ordering of two random intra-group items, for a random group
 */
void EG_random_group_swap(vector<egroup>& egroups, MKP* m)
{
    assert(egroups.size() > 0);

	uniform_int_distribution<int> rint(0, egroups.size()-1);
    // Just an alias
	int* I = m->sortedVars;
    int i = rint(gen);
    egroup gi = egroups[i];

    uniform_int_distribution<int> relem(0, gi.b - gi.a - 1);
    // swap the order of two items within a group
	int a = gi.a + relem(gen);
    int b = gi.a + relem(gen);
	std::swap(I[a], I[b]);
}
