#include "sequential_encoder.h"
#include "../global_data.h"
#include "instance_data.h"

#include <numeric>   //iota
#include <algorithm> //generate
#include <deque>
#include <assert.h>
#include <iostream>

SeqEncoder::SeqEncoder() {};

SeqEncoder::~SeqEncoder() {};

void SeqEncoder::encode_antibandwidth()
{
    if (InstanceData::width < 1 || InstanceData::width > GlobalData::g->n)
    {
        std::cout << "c Non-valid value of w, nothing to encode.\n";
        return;
    }
    do_encode_antibandwidth();
}

void SeqEncoder::do_encode_antibandwidth()
{
    encode_symmetry_break();

    encode_labelling();

    for (std::pair<int, int> nodes : GlobalData::g->edges)
    {
        encode_pair_amo(InstanceData::width, nodes.first, nodes.second);
    }
};

void SeqEncoder::encode_labelling()
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

void SeqEncoder::encode_pair_amo(int w, int node1, int node2)
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

    while (amo_node1.back() <= amo_node1_to && amo_node2.back() <= amo_node2_to)
    {
        encode_glued_amo(amo_node1.begin(), amo_node1.end(), amo_node2.begin(), amo_node2.end());
        amo_node1.push_back(amo_node1.back() + 1);
        amo_node1.pop_front();
        amo_node2.push_back(amo_node2.back() + 1);
        amo_node2.pop_front();
    }
};

void SeqEncoder::encode_eo(vec_int_it it_begin, vec_int_it it_end)
{
    std::vector<int> or_clause;
    int prev = *it_begin;
    or_clause.push_back(prev);
    for (auto i_pos = std::next(it_begin), it_last = std::prev(it_end); i_pos != it_last; ++i_pos)
    {
        int curr = *i_pos;
        int next = InstanceData::vh->get_new_var();
        InstanceData::cc->add_clause({-1 * prev, -1 * curr});
        InstanceData::cc->add_clause({-1 * prev, next});
        InstanceData::cc->add_clause({-1 * curr, next});
        or_clause.push_back(curr);
        prev = next;
    }
    InstanceData::cc->add_clause({-1 * prev, -1 * (*std::prev(it_end))});
    or_clause.push_back(*std::prev(it_end));
    InstanceData::cc->add_clause(or_clause);
};

void SeqEncoder::encode_glued_amo(deq_int_it amo1_begin, deq_int_it amo1_end, deq_int_it amo2_begin, deq_int_it amo2_end)
{
    int prev = *amo1_begin;
    for (auto i_pos = std::next(amo1_begin), amo1_last = std::prev(amo1_end), amo2_last = std::prev(amo2_end); i_pos != amo2_last;)
    {
        int curr = *i_pos;
        int next = InstanceData::vh->get_new_var();
        InstanceData::cc->add_clause({-1 * prev, -1 * curr});
        InstanceData::cc->add_clause({-1 * prev, next});
        InstanceData::cc->add_clause({-1 * curr, next});

        prev = next;
        if (i_pos == amo1_last)
        {
            i_pos = amo2_begin;
        }
        else
        {
            ++i_pos;
        }
    }
    InstanceData::cc->add_clause({-1 * prev, -1 * (*std::prev(amo2_end))});
};
