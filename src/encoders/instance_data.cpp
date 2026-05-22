#include "instance_data.h"
#include "../global_data.h"

#include "sat_solver_cadical.h"

#include "duplex_encoder.h"
#include "product_encoder.h"
#include "reduced_encoder.h"
#include "scl_encoder.h"
#include "sequential_encoder.h"
#include <iostream>

InstanceData::~InstanceData()
{
    if (enc)
        delete enc;
    if (cc)
        delete cc;
    if (vh)
        delete vh;
    if (solver)
        delete solver;
}

int InstanceData::width = 0;

InstanceEncoder *InstanceData::enc = nullptr;
ClauseContainer *InstanceData::cc = nullptr;
VarHandler *InstanceData::vh = nullptr;
SATSolver *InstanceData::solver = nullptr;

std::string InstanceData::get_signature()
{
    return "[w = " + std::to_string(width) + "]";
};

void InstanceData::set_up_encoder()
{
    switch (GlobalData::encode_type)
    {
    case EncodeType::duplex:
        enc = new DuplexEncoder();
        break;
    case EncodeType::product:
        enc = new ProductEncoder();
        break;
    case EncodeType::reduced:
        enc = new ReducedEncoder();
        break;
    case EncodeType::scl:
        enc = new SCLEncoder();
        break;
    case EncodeType::seq:
        enc = new SeqEncoder();
        break;

    default:
        break;
    }
};

void InstanceData::set_up_sat_solver()
{
    switch (GlobalData::sat_solver_type)
    {
    case SATSolverType::CaDiCaL:
        solver = new SATSolverCadical();
        break;

    default:
        break;
    }
};

void InstanceData::setup_for_solving()
{
    setup_for_encoding();

    set_up_sat_solver();
}

void InstanceData::setup_for_encoding()
{
    cc = new ClauseContainer();
    vh = new VarHandler(1, GlobalData::g->n);

    set_up_encoder();
}

void InstanceData::cleanup_encoding()
{
    delete enc;
    delete cc;
    delete vh;
}

void InstanceData::cleanup_solving()
{
    cleanup_encoding();

    delete solver;
}

void InstanceData::export_dimacs(std::ostream &out)
{
    out << "c CNF fomular for graph " << GlobalData::g->graph_name << " with Cyclic Antibandwidth value of " << width << "\n";
    out << "p cnf " << vh->size() << " " << cc->size() << "\n";
    for (const Clause &c : cc->clause_list)
    {
        for (int lit : c)
        {
            out << lit << " ";
        }
        out << "0\n";
    }
}