#ifndef SCL_ENCODER_H
#define SCL_ENCODER_H

#include "instance_encoder.h"
#include <map>
#include <vector>
#include <unordered_map>
#include <cstddef>

struct VectorHash
{
    size_t operator()(const std::vector<int> &v) const
    {
        size_t hash = 0;

        for (int x : v)
        {
            hash ^= std::hash<int>()(x) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }

        return hash;
    }
};

class SCLEncoder : public InstanceEncoder
{
public:
    SCLEncoder();
    ~SCLEncoder() override;

    void encode_antibandwidth() override;

private:
    bool is_debug_mode = false;

    // Use to save aux vars of OBJ-K constraints
    std::unordered_map<std::vector<int>, int, VectorHash> obj_k_aux_vars;

    void do_encode_antibandwidth();

    int get_obj_k_aux_var(std::vector<int> key, bool is_key_exist = false);

    void encode_vertices();
    void encode_labels();
    void encode_exactly_one_product(const std::vector<int> &vars);
    void encode_amo_seq(const std::vector<int> &vars);

    void encode_obj_k();

    void encode_ladder(const std::vector<int> ladder_vars, int width);
    void encode_window(const std::vector<int> window_vars, bool is_first_window, bool is_last_window);
    void connect_windows(const std::vector<int> first_window_vars, const std::vector<int> second_window_vars);
    virtual void connect_ladder(const std::vector<int> first_ladder_vars, const std::vector<int> second_ladder_vars, int width);
};

#endif