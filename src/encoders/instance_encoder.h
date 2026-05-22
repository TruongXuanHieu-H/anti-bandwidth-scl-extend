#ifndef INSTANCE_ENCODER_H
#define INSTANCE_ENCODER_H

class InstanceEncoder
{
public:
    InstanceEncoder();
    virtual ~InstanceEncoder();

    virtual void encode_antibandwidth();

    void encode_symmetry_break();

private:
    void encode_symmetry_break_first_node();
    void encode_symmetry_break_highest_degree_node();
    void encode_symmetry_break_lowest_degree_node();
};

#endif // INSTANCE_ENCODER_H