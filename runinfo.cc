#include "runinfo.h"
#include "mkp.h"
#include "pop.h"
#include "model.h"

#include <cstdio>
#include <chrono>

void runinfo::begin() {
	start = chrono::high_resolution_clock::now();
    bestfound_index = -1;
    bestfound = -1;
    bestknown = -1;
    avgfx = -1;
    gap = -1;
    lpfx = -1;
    nfree_variables = 0;
}

void runinfo::end(population& pop, model& univ, MKP* m) {
	stop = chrono::high_resolution_clock::now();
	chrono::duration<double> elapsed = stop - start;
	etime = elapsed.count();
    // Gather information
    nfree_variables = univ.count_free_variables();
	lpfx = m->lpOptimal;
	bestknown = m->bestFeasible;
	while( !pop.fxpq.empty()) {
		bestfound = pop.fxpq.top().fx;
		bestfound_index = pop.fxpq.top().index;
		avgfx += pop.fxpq.top().fx;
		pop.fxpq.pop();
	}
	avgfx /= pop.m;
	gap = (bestknown - bestfound);//bestknown;

    nevals = m->nevals;
}

void runinfo::print() {
#ifdef DEBUG
    printf("time(s)\t#core\tLPval\t best\t found\t gap\t gap%%\tevals\n");
#endif
	printf("%4.4f\t%d\t%.0f\t %.0f\t %.0f\t %.0f\t %.4f %ld\n",
	etime, nfree_variables, lpfx, bestknown, bestfound,
	gap, 100 * gap/bestknown, nevals);
}
