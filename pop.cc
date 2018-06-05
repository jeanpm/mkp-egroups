#include "pop.h"
#include "global.h"

population::population(int nrows, int ncols, model& m) : m(nrows), n(ncols)
{
    tmp = new int[ncols];
    otmp = new int[ncols];

    fx = new float[nrows];
    p = new int*[nrows];
    for (int i = 0; i < nrows; i++) {
        p[i] = new int[ncols];
        for (int j = 0; j < ncols; j++)
            p[i][j] = (int)(unif(gen) < m.pr[j]);
    }
}

bool population::contains(int* sol, float f)
{
    for (int i = 0; i < m; i++)
    {
        if (fx[i] != f) continue;
        bool exist = true;
        for (int j = 0; j < n ; j++)
        if (p[i][j] != sol[j]) {
            exist = false;
            break;
        }
        if (exist) return true;
    }
    return false;
}

bool population::add_tmp(float new_fx, model& univ)
{
    bool addition_succeeded = false;
    objvalue worst = fxpq.top();
    if (new_fx > fx[worst.index])
    {
        if (!contains(tmp, new_fx)) { // Avoid repeated solutions
            fxpq.pop();
            fx[worst.index] = new_fx;

            for (int j = 0; j < n; j++) {
                univ.updatei(tmp, j, m, new_fx, kp->lpOptimal);
                p[worst.index][j] = tmp[j];
            }
            fxpq.push(objvalue{worst.index, new_fx});
            addition_succeeded = true;
        }
    }
    return addition_succeeded;
}

void population::evaluate(MKP* mkp)
{
    kp = mkp;
    for (int i = 0; i < m; i++)
    {
        fx[i] = mkp_repairFitness(p[i], mkp);
        fxpq.push(objvalue{i, fx[i]});
    }
}

population::~population()
{
    delete[] fx;
    delete[] tmp;
    delete[] otmp;
    for (int i = 0; i < m; i++)
        delete[] p[i];
    delete[] p;
}
