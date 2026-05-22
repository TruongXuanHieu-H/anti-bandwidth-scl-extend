#ifndef SEQ_ENCODER_H
#define SEQ_ENCODER_H

#include "instance_encoder.h"

#include <vector>
#include <deque>

typedef std::vector<int>::iterator vec_int_it;
typedef std::deque<int>::iterator deq_int_it;

class SeqEncoder : public InstanceEncoder
{
public:
    SeqEncoder();
    ~SeqEncoder();

    void encode_antibandwidth() override;

private:
    void do_encode_antibandwidth();

    void encode_labelling();
    void encode_pair_amo(int w, int node1, int node2);

    void encode_eo(vec_int_it it_begin, vec_int_it it_end);
    void encode_glued_amo(deq_int_it amo1_begin, deq_int_it amo1_end, deq_int_it amo2_begin, deq_int_it amo2_end);
};

#endif
