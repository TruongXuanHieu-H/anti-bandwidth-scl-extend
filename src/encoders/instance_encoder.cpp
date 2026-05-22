#include "instance_encoder.h"
#include "../global_data.h"
#include "./instance_data.h"
#include <iostream>

InstanceEncoder::InstanceEncoder() {}
InstanceEncoder::~InstanceEncoder() {}

void InstanceEncoder::encode_antibandwidth()
{
    // This function is a placeholder for the actual encoding logic.
    // It should be implemented in the derived classes.
    std::cerr << "e " << InstanceData::get_signature() << " InstanceEncoder::encode_antibandwidth() is not implemented.\n";
}

void InstanceEncoder::encode_symmetry_break()
{
    if (GlobalData::symmetry_break_strategy == SymmetryBreakingType::FIRST)
    {
        encode_symmetry_break_first_node();
    }
    else if (GlobalData::symmetry_break_strategy == SymmetryBreakingType::HIGHEST_DEGREE)
    {
        encode_symmetry_break_highest_degree_node();
    }
    else if (GlobalData::symmetry_break_strategy == SymmetryBreakingType::LOWEST_DEGREE)
    {
        encode_symmetry_break_lowest_degree_node();
    }
    else
    {
        // No symmetry breaking
    }
}

void InstanceEncoder::encode_symmetry_break_first_node()
{
    std::cout << "c " << InstanceData::get_signature() << " Symmetry breaking first node is applied.\n";
    for (int i = GlobalData::g->n; i > GlobalData::g->n - (GlobalData::g->n / 2); i--)
    {
        InstanceData::cc->add_clause({-1 * int(i)});
    }
}

void InstanceEncoder::encode_symmetry_break_highest_degree_node()
{
    std::cout << "c " << InstanceData::get_signature() << " Symmetry breaking highest degree node is applied.\n";
    int max_node_id = GlobalData::g->find_greatest_outdegree_node();

    for (int i = max_node_id * GlobalData::g->n; i > (max_node_id * GlobalData::g->n) - (GlobalData::g->n / 2); i--)
    {
        InstanceData::cc->add_clause({-1 * int(i)});
    }
}

void InstanceEncoder::encode_symmetry_break_lowest_degree_node()
{
    std::cout << "c " << InstanceData::get_signature() << " Symmetry breaking lowest degree node is applied.\n";
    int min_node_id = GlobalData::g->find_smallest_outdegree_node();

    for (int i = min_node_id * GlobalData::g->n; i > (min_node_id * GlobalData::g->n) - (GlobalData::g->n / 2); i--)
    {
        InstanceData::cc->add_clause({-1 * int(i)});
    }
}