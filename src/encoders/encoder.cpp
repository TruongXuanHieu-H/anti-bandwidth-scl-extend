#include "encoder.h"

#include <iostream>
#include <assert.h>
#include <limits>

Encoder::Encoder(Graph *graph, ClauseContainer *clause_container, VarHandler *var_handler) : cv(clause_container), g(graph), vh(var_handler) {};

Encoder::~Encoder() {};

void Encoder::encode_antibandwidth(int w, const std::vector<std::pair<int, int>> &node_pairs)
{
    if (w < 1 || w > g->n)
    {
        std::cout << "c Non-valid value of w, nothing to encode." << std::endl;
        return;
    }
    do_encode_antibandwidth(w, node_pairs);
};

void Encoder::encode_symmetry_break()
{
    if (symmetry_break_point == SymmetryBreakingType::FIRST)
    {
        encode_symmetry_break_on_firstnode();
    }
    else if (symmetry_break_point == SymmetryBreakingType::HIGHEST_DEGREE)
    {
        encode_symmetry_break_on_maxnode();
    }
    else if (symmetry_break_point == SymmetryBreakingType::LOWEST_DEGREE)
    {
        encode_symmetry_break_on_minnode();
    }
    else
    {
        // No symmetry breaking
    }
}

void Encoder::encode_symmetry_break_on_firstnode()
{
    for (int i = g->n; i > g->n - (g->n / 2); i--)
    {
        cv->add_clause({-1 * int(i)});
    }
};

void Encoder::encode_symmetry_break_on_maxnode()
{
    int max_node_id = g->find_greatest_outdegree_node();

    for (int i = max_node_id * g->n; i > (max_node_id * g->n) - (g->n / 2); i--)
    {
        cv->add_clause({-1 * int(i)});
    }
};

void Encoder::encode_symmetry_break_on_minnode()
{
    int max_node_id = g->find_smallest_outdegree_node();

    for (int i = max_node_id * g->n; i > (max_node_id * g->n) - (g->n / 2); i--)
    {
        cv->add_clause({-1 * int(i)});
    }
};

int Encoder::size() const
{
    return cv->size();
};

int Encoder::vars_size() const
{
    return do_vars_size();
};
