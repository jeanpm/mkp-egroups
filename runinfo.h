#ifndef RUNINFO_H
#define RUNINFO_H

#include <chrono>

#include "mkp.h"
#include "pop.h"
#include "model.h"

using namespace std;

struct runinfo {
	int   bestfound_index;
	float bestfound;
	float bestknown;
	float avgfx;
	float gap;
	float lpfx;
	int   nfree_variables;
    long  nevals;

	chrono::high_resolution_clock::time_point start;
	chrono::high_resolution_clock::time_point stop;

	double etime;

	void begin();

	void end(population& pop, model& univ, MKP* m);

	void print();
};
#endif
