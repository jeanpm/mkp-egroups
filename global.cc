#include "global.h"

#include "mkp.h"

long MAXEVALS = 0;

int EXIT_STATUS = 0;

bool stop_criterion(MKP* m)
{
    EXIT_STATUS = CONTINUE_SEARCH;
    if (m->nevals >= MAXEVALS) {
        EXIT_STATUS = MAXEVALS_REACHED;
        return true;
    } else if (m->bestFound >= m->bestFeasible) {
        EXIT_STATUS = BESTKNOWN_FOUND;
        return true;
    }
    return false;
}

bool finished()
{
    return (EXIT_STATUS == MAXEVALS_REACHED) || (EXIT_STATUS == BESTKNOWN_FOUND);
}
