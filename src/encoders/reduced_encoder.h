#ifndef REDUCED_ENCODER_H
#define REDUCED_ENCODER_H

#include "instance_encoder.h"

#include <vector>
#include <deque>

typedef std::vector<int>::iterator vec_int_it;
typedef std::deque<int>::iterator deq_int_it;

class ReducedEncoder : public InstanceEncoder
{
public:
    ReducedEncoder();
    ~ReducedEncoder();

    void encode_antibandwidth() override;

private:
    void do_encode_antibandwidth();

    void encode_labelling();
    void encode_pair_amo(int w, int node1, int node2);

    void encode_eo(vec_int_it it_begin, vec_int_it it_end);
    void encode_glued_first_amo(deq_int_it amo1_begin, deq_int_it amo1_end, deq_int_it amo2_begin, deq_int_it amo2_end);
    void encode_next_window(deq_int_it amo1_begin, deq_int_it amo1_end, deq_int_it amo2_begin, deq_int_it amo2_end, int new_g1, int new_g2);
};

#endif
