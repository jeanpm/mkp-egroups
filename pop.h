#ifndef POP_H
#define POP_H

#include "model.h"
#include "mkp.h"

#include <queue>

struct objvalue {
	int index;
	float fx;
	bool operator<(const objvalue& e) const {
		return fx > e.fx;
	}
};

struct population {
	int** p;
	int   m;
	int   n;
	float* fx;
	int*  tmp;  // temporary memory for solutions
    int*  otmp; // temporary memory for opposite solutions
	std::priority_queue<objvalue> fxpq;
	MKP* kp;

    population(int nrows, int ncols, model& m);

    bool contains(int* sol, float f);

    bool add_tmp(float new_fx, model& univ);

    void evaluate(MKP* mkp);

	~population();
};
#endif
