#ifndef ABP_ENCODER_H
#define ABP_ENCODER_H

#include "../graph/graph.h"
#include "../enum/encode_type.h"
#include "instance_encoder.h"
#include "clause_container.h"
#include "sat_solver.h"

#include <string>

class ABWInstance
{
public:
    ABWInstance(int width);
    virtual ~ABWInstance();

    int encode_and_solve_abp();
    void encode_and_print_dimacs();

private:
    int SAT_res = 0;

    int verify_solution();
};
#endif