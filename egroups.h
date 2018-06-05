#ifndef EGROUPS_H
#define EGROUPS_H

/*
 An efficiency group is defined by items whose efficiencies are equal. Items
 whose efficiencies are unique, do  not compose groups.

 Given a non-ascending ordering for items, all groups can be identified in O(n).

 If efficiencies are preprocessed (rounded down) the groups naturally change
 */

#include "mkp.h"

#include <vector>
#include <cmath>

enum class GroupMutation
{
    RANDOM_SWAP,    // Swap the order of two itens in a random group
    RANDOM_SHUFFLE, // Shuffle the order of a random group
    SHUFFLE_ALL,     // Shuflle the order of all groups
    NONE
};

/*
 Given an ordering I for the knapsack items, an efficiency group is represented
 by initial and final indices [a, b).
 */
struct egroup {int a, b;};

/*
 Round a double to a certain number of decimal cases
 */
double round_to(double value, int dcases);

/*
 Read the MKP efficiencies and produces the groups. The efficiencies are rounded
 down to dcases.
 */
void EG_getEfficiencyGroups(std::vector<egroup>& eg, MKP* m, int dcases);

/*
 To ignore the efficiency groups use this function as mutation operator
 */
void EG_donothing(std::vector<egroup>& egroups, MKP* m);

/*
 Shuffle the ordering of intra-group items, for all groups.
 */
void EG_shuffle_all(std::vector<egroup>& egroups, MKP* m);

/*
 Shuffle the ordering of intra-group items, for a random group
 */
void EG_random_group_shuffle(std::vector<egroup>& egroups, MKP* m);

/*
 Swap the ordering of two random intra-group items, for a random group
 */
void EG_random_group_swap(std::vector<egroup>& egroups, MKP* m);


#endif
