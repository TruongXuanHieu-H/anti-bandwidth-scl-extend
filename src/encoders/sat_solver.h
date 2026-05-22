#ifndef SAT_SOLVER_H
#define SAT_SOLVER_H

#include "clause_container.h"
#include <vector>

class SATSolver
{
public:
    SATSolver() = default;
    virtual ~SATSolver() = default;

    virtual void set_up_solver() = 0;
    virtual void add_clause(const Clause &c) = 0;
    virtual int solve() = 0;
    virtual std::vector<int> extract_result() = 0;
    virtual void clear_solver() = 0;
};

#endif // SAT_SOLVER_H