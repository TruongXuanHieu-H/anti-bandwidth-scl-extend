#include "sat_solver_cadical.h"
#include "instance_data.h"
#include "../global_data.h"
#include <iostream>
#include <csignal>

SATSolverCadical::SATSolverCadical()
{
    set_up_solver();
}

SATSolverCadical::~SATSolverCadical()
{
    clear_solver();
}

void SATSolverCadical::set_up_solver()
{
    solver = new CaDiCaL::Solver();
    std::cout << "c " << InstanceData::get_signature() << " Initializing CaDiCaL (version " << solver->version() << ").\n";
    std::string sat_configuration = "sat";
    int res = solver->configure(sat_configuration.data());
    std::cout << "c " << InstanceData::get_signature() << " Configuring CaDiCaL as --" << sat_configuration << " (" << res << ").\n";
}

void SATSolverCadical::clear_solver()
{
    delete solver;
    solver = nullptr;
}

void SATSolverCadical::add_clause(const Clause &c)
{
    for (const auto &lit : c)
    {
        solver->add(lit);
    }
    solver->add(0); // End of clause
}

int SATSolverCadical::solve()
{
    return solver->solve();
}

std::vector<int> SATSolverCadical::extract_result()
{
    std::vector<int> result(GlobalData::g->n, 0);
    for (int node = 0; node < GlobalData::g->n; ++node)
    {
        for (int label = 1; label <= GlobalData::g->n; ++label)
        {
            int res = solver->val(node * GlobalData::g->n + label);
            if (res > 0)
            {
                if (result[node] != 0)
                {
                    std::cerr << "e " << InstanceData::get_signature() << " Error, the solution is not a labelling: more than one label assigned for node " << node + 1 << ".\n";
                    raise(SIGABRT);
                }
                result[node] = label;
            }
        }
    }

    return result;
}
