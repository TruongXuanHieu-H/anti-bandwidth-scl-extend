#include "scl_encoder.h"

#include <iostream>
#include <numeric>
#include <algorithm>
#include <assert.h>
#include <cmath>

SCLEncoder::SCLEncoder(Graph *g, ClauseContainer *cc, VarHandler *vh) : Encoder(g, cc, vh)
{
}

SCLEncoder::~SCLEncoder() {}

int SCLEncoder::get_obj_k_aux_var(int first, int last)
{

    auto pair = obj_k_aux_vars.find({first, last});

    if (pair != obj_k_aux_vars.end())
        return pair->second;

    if (first == last)
        return first;

    int new_obj_k_aux_var = vh->get_new_var();
    obj_k_aux_vars.insert({{first, last}, new_obj_k_aux_var});
    return new_obj_k_aux_var;
}

int SCLEncoder::do_vars_size() const
{
    return vh->size();
};

void SCLEncoder::do_encode_antibandwidth(int w, const std::vector<std::pair<int, int>> &node_pairs)
{
    aux_vars.clear();
    obj_k_aux_vars.clear();

    vertices_aux_var = g->n * g->n;
    labels_aux_var = vertices_aux_var + g->n * g->n;

    if (symmetry_break_point == SymmetryBreakingType::FIRST)
    {
        encode_symmetry_break();
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
    // std::cout << "c\tEncode symmetry breaking with option: " << symmetry_break_point << "." << std::endl;

    encode_vertices();
    // encode_labels();
    encode_obj_k(w);

    // Prevent error when build due to unused variables
    (void)node_pairs;
};

void SCLEncoder::encode_vertices()
{
    for (int i = 0; i < g->n; i++)
    {
        std::vector<int> node_vertices_eo(g->n);
        int j = 0;

        std::generate(node_vertices_eo.begin(), node_vertices_eo.end(), [this, &j, i]()
                      { return (j++ * g->n) + i + 1; });

        encode_exactly_one_product(node_vertices_eo);
    }
}

void SCLEncoder::encode_labels()
{
    for (int i = 0; i < g->n; i++)
    {
        std::vector<int> node_labels_eo(g->n);
        std::iota(node_labels_eo.begin(), node_labels_eo.end(), (i * g->n) + 1);

        encode_exactly_one_product(node_labels_eo);
    }
}

void SCLEncoder::encode_exactly_one_product(const std::vector<int> &vars)
{
    if (vars.size() < 2)
        return;
    if (vars.size() == 2)
    {
        // simplifies to vars[0] /\ -1*vars[0], in case vars[0] == vars[1]
        cv->add_clause({vars[0], vars[1]});
        cv->add_clause({-1 * vars[0], -1 * vars[1]});
        return;
    }

    int len = vars.size();
    int p = std::ceil(std::sqrt(len));
    int q = std::ceil((float)len / (float)p);

    std::vector<int> u_vars;
    std::vector<int> v_vars;
    for (int i = 1; i <= p; ++i)
    {
        int new_var = vh->get_new_var();
        u_vars.push_back(new_var);
        aux_vars.insert({new_var, new_var});
    }
    for (int j = 1; j <= q; ++j)
    {
        int new_var = vh->get_new_var();
        v_vars.push_back(new_var);
        aux_vars.insert({new_var, new_var});
    }

    int i, j;
    std::vector<int> or_clause = std::vector<int>();
    for (int idx = 0; idx < (int)vars.size(); ++idx)
    {
        i = std::floor(idx / p);
        j = idx % p;

        cv->add_clause({-1 * vars[idx], v_vars[i]});
        cv->add_clause({-1 * vars[idx], u_vars[j]});

        or_clause.push_back(vars[idx]);
    }
    cv->add_clause(or_clause);

    encode_amo_seq(u_vars);
    encode_amo_seq(v_vars);
};

void SCLEncoder::encode_amo_seq(const std::vector<int> &vars)
{
    if (vars.size() < 2)
        return;

    int prev = vars[0];

    for (int idx = 1; idx < (int)vars.size() - 1; ++idx)
    {
        int curr = vars[idx];
        int next = vh->get_new_var();
        aux_vars.insert({next, next});
        cv->add_clause({-1 * prev, -1 * curr});
        cv->add_clause({-1 * prev, next});
        cv->add_clause({-1 * curr, next});

        prev = next;
    }
    cv->add_clause({-1 * prev, -1 * vars[vars.size() - 1]});
};

void SCLEncoder::encode_obj_k(int w)
{
    for (int i = 0; i < (int)g->n; i++)
    {
        encode_stair(i, w);
    }

    for (auto edge : g->edges)
    {
        glue_stair(edge.first - 1, edge.second - 1, w);
    }
}

void SCLEncoder::encode_stair(int stair, int w)
{
    if (is_debug_mode)
        std::cout << "Encode stair " << stair << " with width " << w << std::endl;

    for (int gw = 0; gw < ceil((float)g->n / w); gw++)
    {
        if (is_debug_mode)
            std::cout << "Encode window " << gw << std::endl;
        encode_window(gw, stair, w);
    }

    for (int gw = 0; gw < ceil((float)g->n / w) - 1; gw++)
    {
        if (is_debug_mode)
            std::cout << "Glue window " << gw << " with window " << gw + 1 << std::endl;
        glue_window(gw, stair, w);
    }

    std::vector<std::pair<int, int>> windows = {};
    int number_windows = ceil((float)g->n / w);

    for (int i = 0; i < number_windows; i++)
    {
        int stair_anchor = stair * (int)g->n;
        int window_anchor = i * (int)w;
        if (window_anchor + w > g->n)
            windows.push_back({stair_anchor + window_anchor + 1, stair_anchor + g->n});
        else
            windows.push_back({stair_anchor + window_anchor + 1, stair_anchor + window_anchor + w});
    }

    std::vector<int> alo_clause = {};
    for (int i = 0; i < number_windows; i++)
    {
        int first_window_aux_var = get_obj_k_aux_var(windows[i].first, windows[i].second);
        alo_clause.push_back(first_window_aux_var);
        for (int j = i + 1; j < number_windows; j++)
        {
            int second_window_aux_var = get_obj_k_aux_var(windows[j].first, windows[j].second);
            cv->add_clause({-first_window_aux_var, -second_window_aux_var});
        }
    }
    cv->add_clause(alo_clause);
}

void SCLEncoder::encode_window(int window, int stair, int w)
{
    if (window == 0)
    {
        // Encode the first window, which only have lower part
        int lastVar = stair * (int)g->n + window * (int)w + w;

        for (int i = w - 1; i >= 1; i--)
        {
            int var = stair * (int)g->n + window * (int)w + i;
            cv->add_clause({-var, get_obj_k_aux_var(var, lastVar)});
        }

        for (int i = w; i >= 2; i--)
        {
            int var = stair * (int)g->n + window * (int)w + i;
            cv->add_clause({-get_obj_k_aux_var(var, lastVar), get_obj_k_aux_var(var - 1, lastVar)});
        }

        for (int i = 1; i < (int)w; i++)
        {
            int var = stair * (int)g->n + window * (int)w + i;
            int main = get_obj_k_aux_var(var, lastVar);
            int sub = get_obj_k_aux_var(var + 1, lastVar);
            cv->add_clause({var, sub, -main});
        }

        for (int i = 1; i < (int)w; i++)
        {
            int var = stair * (int)g->n + window * (int)w + i;
            cv->add_clause({-var, -get_obj_k_aux_var(var + 1, lastVar)});
        }
    }
    else if (window == ceil((float)g->n / w) - 1)
    {
        // Encode the last window, which only have upper part and may have width lower than w
        int firstVar = stair * (int)g->n + window * (int)w + 1;

        if ((window + 1) * w > g->n)
        {
            int real_w = g->n % w;
            // Upper part
            for (int i = 2; i <= real_w; i++)
            {
                int reverse_var = stair * (int)g->n + window * (int)w + i;
                cv->add_clause({-reverse_var, get_obj_k_aux_var(firstVar, reverse_var)});
            }

            for (int i = real_w - 1; i > 0; i--)
            {
                int reverse_var = stair * (int)g->n + window * (int)w + real_w - i;
                cv->add_clause({-get_obj_k_aux_var(firstVar, reverse_var), get_obj_k_aux_var(firstVar, reverse_var + 1)});
            }

            for (int i = 0; i < (int)real_w - 1; i++)
            {
                int var = stair * (int)g->n + window * (int)w + real_w - i;
                int main = get_obj_k_aux_var(firstVar, var);
                int sub = get_obj_k_aux_var(firstVar, var - 1);
                cv->add_clause({sub, var, -main});
            }

            for (int i = real_w; i > 1; i--)
            {
                int reverse_var = stair * (int)g->n + window * (int)w + i;
                cv->add_clause({-reverse_var, -get_obj_k_aux_var(firstVar, reverse_var - 1)});
            }
        }
        else
        {
            // Upper part
            for (int i = 2; i <= (int)w; i++)
            {
                int reverse_var = stair * (int)g->n + window * (int)w + i;
                cv->add_clause({-reverse_var, get_obj_k_aux_var(firstVar, reverse_var)});
            }

            for (int i = w - 1; i >= 1; i--)
            {
                int reverse_var = stair * (int)g->n + window * (int)w + w - i;
                cv->add_clause({-get_obj_k_aux_var(firstVar, reverse_var), get_obj_k_aux_var(firstVar, reverse_var + 1)});
            }

            for (int i = 0; i < (int)w - 1; i++)
            {
                int var = stair * (int)g->n + window * (int)w + w - i;
                int main = get_obj_k_aux_var(firstVar, var);
                int sub = get_obj_k_aux_var(firstVar, var - 1);
                cv->add_clause({sub, var, -main});
            }

            for (int i = (int)w; i > 1; i--)
            {
                int reverse_var = stair * (int)g->n + window * (int)w + i;
                cv->add_clause({-reverse_var, -get_obj_k_aux_var(firstVar, reverse_var - 1)});
            }
        }
    }
    else
    {
        // Encode the middle windows, which have both upper and lower path, and always have width w

        // Upper part
        int firstVar = stair * (int)g->n + window * (int)w + 1;
        for (int i = 2; i <= (int)w; i++)
        {
            int reverse_var = stair * (int)g->n + window * (int)w + i;
            cv->add_clause({-reverse_var, get_obj_k_aux_var(firstVar, reverse_var)});
        }

        for (int i = w - 1; i >= 1; i--)
        {
            int reverse_var = stair * (int)g->n + window * (int)w + w - i;
            cv->add_clause({-get_obj_k_aux_var(firstVar, reverse_var), get_obj_k_aux_var(firstVar, reverse_var + 1)});
        }

        for (int i = 0; i < (int)w - 1; i++)
        {
            int var = stair * (int)g->n + window * (int)w + w - i;
            int main = get_obj_k_aux_var(firstVar, var);
            int sub = get_obj_k_aux_var(firstVar, var - 1);
            cv->add_clause({sub, var, -main});
        }

        for (int i = (int)w; i > 1; i--)
        {
            int reverse_var = stair * (int)g->n + window * (int)w + i;
            cv->add_clause({-reverse_var, -get_obj_k_aux_var(firstVar, reverse_var - 1)});
        }

        // Lower part
        int lastVar = stair * (int)g->n + window * (int)w + w;
        for (int i = w - 1; i >= 1; i--)
        {
            int var = stair * (int)g->n + window * (int)w + i;
            cv->add_clause({-var, get_obj_k_aux_var(var, lastVar)});
        }

        for (int i = w; i >= 2; i--)
        {
            int var = stair * (int)g->n + window * (int)w + i;
            cv->add_clause({-get_obj_k_aux_var(var, lastVar), get_obj_k_aux_var(var - 1, lastVar)});
        }

        for (int i = 1; i < (int)w; i++)
        {
            int var = stair * (int)g->n + window * (int)w + i;
            int main = get_obj_k_aux_var(var, lastVar);
            int sub = get_obj_k_aux_var(var + 1, lastVar);
            cv->add_clause({var, sub, -main});
        }

        // Can be disable
        // for (int i = 1; i < (int)w; i++)
        // {
        //     int var = stair * (int)g->n + window * (int)w + i;
        //     cv->add_clause({-var, -GetEncodedAuxVar(auxStartVarLP + var + 1)});
        //     num_obj_k_constraints++;
        // }
    }
}

/*
 * Glue adjacent windows with each other.
 * Using lower part of the previous window and upper part of the next window
 * as anchor points to glue.
 */
void SCLEncoder::glue_window(int window, int stair, int w)
{
    /*  The stair look like this:
     *      Window 1        Window 2        Window 3        Window 4
     *      1   2   3   |               |               |
     *          2   3   |   4           |               |
     *              3   |   4   5       |               |
     *                  |   4   5   6   |               |
     *                  |       5   6   |   7           |
     *                  |           6   |   7   8       |
     *                  |               |   7   8   9   |
     *                  |               |       8   9   |   10
     *                  |               |           9   |   10  11
     *
     * If the next window has width of w, then we only encode w - 1 register bits (because
     * NSC only define w - 1 register bits), else we encode using number of register bit
     * equal to width of the next window.
     */
    if ((window + 2) * w > g->n)
    {
        int real_w = g->n % w;
        for (int i = 1; i <= real_w; i++)
        {
            int first_reverse_var = stair * (int)g->n + (window + 1) * (int)w + 1;
            int last_var = stair * (int)g->n + window * (int)w + w;

            int reverse_var = stair * (int)g->n + (window + 1) * (int)w + i;
            int var = stair * (int)g->n + window * (int)w + i + 1;

            cv->add_clause({-get_obj_k_aux_var(var, last_var), -get_obj_k_aux_var(first_reverse_var, reverse_var)});
        }
    }
    else
    {
        for (int i = 1; i < (int)w; i++)
        {
            int first_reverse_var = stair * (int)g->n + (window + 1) * (int)w + 1;
            int last_var = stair * (int)g->n + window * (int)w + w;

            int reverse_var = stair * (int)g->n + (window + 1) * (int)w + i;
            int var = stair * (int)g->n + window * (int)w + i + 1;

            cv->add_clause({-get_obj_k_aux_var(var, last_var), -get_obj_k_aux_var(first_reverse_var, reverse_var)});
        }
    }
}

void SCLEncoder::glue_stair(int stair1, int stair2, int w)
{
    if (is_debug_mode)
        std::cout << "Glue stair " << stair1 << " with stair " << stair2 << std::endl;
    int number_step = g->n - w + 1;
    for (int i = 0; i < number_step; i++)
    {
        int mod = i % w;
        int subset = i / w;
        if (mod == 0)
        {
            int firstVar = get_obj_k_aux_var(stair1 * g->n + subset * w + 1, stair1 * g->n + subset * w + w);
            int secondVar = get_obj_k_aux_var(stair2 * g->n + subset * w + 1, stair2 * g->n + subset * w + w);
            cv->add_clause({-firstVar, -secondVar});
        }
        else
        {
            int firstVar = get_obj_k_aux_var(stair1 * g->n + subset * w + 1 + mod, stair1 * g->n + subset * w + w);
            int secondVar = get_obj_k_aux_var(stair1 * g->n + subset * w + w + 1, stair1 * g->n + subset * w + w + mod);
            int thirdVar = get_obj_k_aux_var(stair2 * g->n + subset * w + 1 + mod, stair2 * g->n + subset * w + w);
            int forthVar = get_obj_k_aux_var(stair2 * g->n + subset * w + w + 1, stair2 * g->n + subset * w + w + mod);
            cv->add_clause({-firstVar, -thirdVar});
            cv->add_clause({-firstVar, -forthVar});
            cv->add_clause({-secondVar, -thirdVar});
            cv->add_clause({-secondVar, -forthVar});
        }
    }
}
