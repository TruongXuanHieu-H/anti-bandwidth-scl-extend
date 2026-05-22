#include "reduced_encoder.h"
#include "../global_data.h"
#include "instance_data.h"

#include <numeric>   //iota
#include <algorithm> //generate
#include <assert.h>
#include <iostream>

ReducedEncoder::ReducedEncoder() {};

ReducedEncoder::~ReducedEncoder() {};

void ReducedEncoder::encode_antibandwidth()
{
    if (InstanceData::width < 1 || InstanceData::width > GlobalData::GlobalData::g->n)
    {
        std::cout << "c Non-valid value of w, nothing to encode.\n";
        return;
    }
    do_encode_antibandwidth();
}

void ReducedEncoder::do_encode_antibandwidth()
{
    encode_symmetry_break();

    encode_labelling();

    for (std::pair<int, int> nodes : GlobalData::g->edges)
    {
        encode_pair_amo(InstanceData::width, nodes.first, nodes.second);
    }
};

// Enforces that each node takes maximum one label and each label belongs to maximum one node.
void ReducedEncoder::encode_labelling()
{
    for (int i = 0; i < GlobalData::g->n; i++)
    {
        std::vector<int> node_label_eo(GlobalData::g->n);
        std::iota(node_label_eo.begin(), node_label_eo.end(), (i * GlobalData::g->n) + 1);
        encode_eo(node_label_eo.begin(), node_label_eo.end());
    }

    for (int i = 0; i < GlobalData::g->n; i++)
    {
        std::vector<int> label_node_eo(GlobalData::g->n);
        int j = 0;
        std::generate(label_node_eo.begin(), label_node_eo.end(), [this, &j, i]()
                      { return (j++ * GlobalData::g->n) + i + 1; });
        encode_eo(label_node_eo.begin(), label_node_eo.end());
    }
};

void ReducedEncoder::encode_pair_amo(int w, int node1, int node2)
{
    assert(node1 != node2);
    assert(0 < node1 && node1 <= GlobalData::g->n);
    assert(0 < node2 && node2 <= GlobalData::g->n);
    std::deque<int> amo_node1(w);
    std::deque<int> amo_node2(w);
    int amo_node1_from = (node1 - 1) * GlobalData::g->n + 1;
    int amo_node2_from = (node2 - 1) * GlobalData::g->n + 1;
    int amo_node1_to = node1 * GlobalData::g->n; // last variable belonging to node1
    int amo_node2_to = node2 * GlobalData::g->n;

    std::iota(amo_node1.begin(), amo_node1.end(), amo_node1_from);
    std::iota(amo_node2.begin(), amo_node2.end(), amo_node2_from);

    encode_glued_first_amo(amo_node1.begin(), amo_node1.end(), amo_node2.begin(), amo_node2.end());
    while (amo_node1.back() < amo_node1_to && amo_node2.back() < amo_node2_to)
    {
        encode_next_window(amo_node1.begin(), amo_node1.end(), amo_node2.begin(), amo_node2.end(), amo_node1.back() + 1, amo_node2.back() + 1);
        amo_node1.push_back(amo_node1.back() + 1);
        amo_node1.pop_front();
        amo_node2.push_back(amo_node2.back() + 1);
        amo_node2.pop_front();
    }
};

void ReducedEncoder::encode_eo(vec_int_it it_begin, vec_int_it it_end)
{
    std::vector<int> or_clause;
    for (auto i_pos = it_begin, it_last = std::prev(it_end); i_pos != it_last; ++i_pos)
    {
        or_clause.push_back(*i_pos);
        for (auto j_pos = std::next(i_pos); j_pos != it_end; ++j_pos)
        {
            InstanceData::cc->add_clause({-1 * (*i_pos), -1 * (*j_pos)});
        }
    }
    or_clause.push_back(*std::prev(it_end));
    InstanceData::cc->add_clause(or_clause);
};

void ReducedEncoder::encode_glued_first_amo(deq_int_it amo1_begin, deq_int_it amo1_end, deq_int_it amo2_begin, deq_int_it amo2_end)
{
    int i_count = 1;
    for (auto i_pos = amo1_begin; i_pos != amo1_end; ++i_pos, ++i_count)
    {
        int j_count = 1;
        for (auto j_pos = amo2_begin; j_pos != amo2_end; ++j_pos, ++j_count)
        {
            if (i_count != j_count)
            {
                InstanceData::cc->add_clause({-1 * (*i_pos), -1 * (*j_pos)});
            }
        }
    }
};

void ReducedEncoder::encode_next_window(deq_int_it amo1_begin, deq_int_it amo1_end, deq_int_it amo2_begin, deq_int_it amo2_end, int new_g1, int new_g2)
{
    for (auto i_pos = std::next(amo1_begin); i_pos != amo1_end; ++i_pos)
    {
        InstanceData::cc->add_clause({-1 * (*i_pos), -1 * new_g2});
    }
    for (auto i_pos = std::next(amo2_begin); i_pos != amo2_end; ++i_pos)
    {
        InstanceData::cc->add_clause({-1 * new_g1, -1 * (*i_pos)});
    }
};
