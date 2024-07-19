#include "duplex_nsc_encoder.h"
#include "math_extension.h"

#include <iostream>
#include <numeric>   //iota
#include <algorithm> //reverse
#include <assert.h>
#include <cmath> //floor,ceil
#include <map>

/*
    Find more details about New Sequential Counter Encoding in
    document named "New Sequential Counter Encoding for Cardinality
    Constraints" - PhD To Van Khanh et al.
*/
namespace SATABP
{
    bool is_debug_mode = false;

    int vertices_aux_var = 0;
    int labels_aux_var = 0;

    // Use to save aux vars of LABELS and VERTICES constraints
    std::map<int, int> aux_vars = {};
    int eo_constraints = 0;

    // Use to save aux vars of OBJ-K constraints
    std::map<std::pair<int, int>, int> obj_k_aux_vars;
    int obj_k_constraints = 0;

    DuplexNSCEncoder::DuplexNSCEncoder(Graph *g, ClauseContainer *cc, VarHandler *vh) : Encoder(g, cc, vh)
    {
    }

    DuplexNSCEncoder::~DuplexNSCEncoder() {}

    int DuplexNSCEncoder::get_aux_var(int symbolicAuxVar)
    {
        auto pair = aux_vars.find(symbolicAuxVar);

        if (pair != aux_vars.end())
            return pair->second;

        int new_aux_var = vh->get_new_var();
        aux_vars.insert({symbolicAuxVar, new_aux_var});
        return new_aux_var;
    }

    int DuplexNSCEncoder::get_obj_k_aux_var(int first, int last)
    {

        auto pair = obj_k_aux_vars.find({first, last});

        if (pair != obj_k_aux_vars.end())
            return pair->second;

        int new_obj_k_aux_var = vh->get_new_var();
        obj_k_aux_vars.insert({{first, last}, new_obj_k_aux_var});
        return new_obj_k_aux_var;
    }

    int DuplexNSCEncoder::do_vars_size() const
    {
        return vh->size();
    };

    void DuplexNSCEncoder::do_encode_antibandwidth(unsigned w, const std::vector<std::pair<int, int>> &node_pairs)
    {
        eo_constraints = 0;
        obj_k_constraints = 0;
        aux_vars.clear();
        obj_k_aux_vars.clear();

        vertices_aux_var = g->n * g->n;
        labels_aux_var = vertices_aux_var + g->n * g->n;

        encode_vertices();
        encode_labels();
        encode_obj_k(w);

        // Prevent error when build due to unused variables
        (void)node_pairs;
        (void)w;
        std::cout << "Aux var: " << aux_vars.size() << std::endl;
        std::cout << "Obj k aux var: " << obj_k_aux_vars.size() << std::endl;
    };

    void DuplexNSCEncoder::encode_vertices()
    {
        /*
            Encode that each label can only be assigned to one node.
            Assume there are 30 nodes, the constraints looks like:
            1 + 31 + 61 + ... + 871 = 1
            2 + 32 + 62 + ... + 872 = 1
            ...
            30 + 60 + 90 + ... + 900 = 1
        */
        for (unsigned i = 0; i < g->n; i++)
        {
            std::vector<int> node_vertices_eo(g->n);
            int j = 0;

            std::generate(node_vertices_eo.begin(), node_vertices_eo.end(), [this, &j, i]()
                          { return (j++ * g->n) + i + 1; });
            encode_exactly_one(node_vertices_eo, vertices_aux_var + i * g->n);
        }
    }

    void DuplexNSCEncoder::encode_labels()
    {
        /*
            Encode that each vertex can only take one and only one label.
            Assume there are 30 nodes, the constraints looks like:
            1 + 2 + 3 + ... + 30 = 1
            31 + 32 + 33 + ... + 60 = 1
            ...
            871 + 872 + 873 + ... + 900 = 1
        */
        for (unsigned i = 0; i < g->n; i++)
        {
            std::vector<int> node_labels_eo(g->n);
            std::iota(node_labels_eo.begin(), node_labels_eo.end(), (i * g->n) + 1);
            encode_exactly_one(node_labels_eo, labels_aux_var + i * g->n);
        }
    }

    void DuplexNSCEncoder::encode_exactly_one(std::vector<int> listVars, int auxVar)
    {
        // Exactly one variables in listVars is True
        // Using NSC to encode AMO and ALO, EO = AMO and ALO
        // Auxilian variables starts from auxVar + 1

        int listVarsSize = listVars.size();

        // Constraint 1: Xi -> R(i, 1) for i in [1, n - 1]
        // In CNF: not(Xi) or R(i, 1)
        for (int i = 1; i <= listVarsSize - 1; i++)
        {
            cv->add_clause({-listVars[i - 1], get_aux_var(auxVar + i)});
        }

        // Constraint 2: R(i-1, 1) -> R(i, 1) for i in [2, n - 1]
        // In CNF: not(R(i-1, 1)) or R(i, 1)
        for (int i = 2; i <= listVarsSize - 1; i++)
        {
            cv->add_clause({-(get_aux_var(auxVar + i - 1)), get_aux_var(auxVar + i)});
        }

        // Constraint 3: Since k = 1 (Exactly 1 constraint), this constraint is empty and then skipped.

        // Constraint 4: not(Xi) and not(R(i-1, 1)) -> not(R(i, 1)) for i in [2, n - 1]
        // In CNF: Xi or R(i-1, 1) or not (R(i, 1))
        for (int i = 2; i <= listVarsSize - 1; i++)
        {
            cv->add_clause({listVars[i - 1], get_aux_var(auxVar + i - 1), -(get_aux_var(auxVar + i))});
        }

        // Constraint 5: not(X1) -> not(R(1,1))
        // In CNF: X1 or not(R(1,1))
        cv->add_clause({listVars[0], -(get_aux_var(auxVar + 1))});

        // Constraint 6: Since k = 1 (Exactly 1 constraint), this constraint is empty and then skipped.

        // Constraint 7: (At Least k) R(n-1, 1) or Xn
        // In CNF: R(n-1, 1) or Xn
        cv->add_clause({get_aux_var(auxVar + listVarsSize - 1), listVars[listVarsSize - 1]});

        // Constraint 8: (At Most k) Xi -> not(R(i-1,1)) for i in [k + 1, n]
        // In CNF: not(Xi) or not(R(i-1,1))
        for (int i = 2; i <= listVarsSize; i++)
        {
            cv->add_clause({-listVars[i - 1], -(get_aux_var(auxVar + i - 1))});
        }
    }

    void DuplexNSCEncoder::encode_obj_k(unsigned w)
    {
        for (int i = 0; i < (int)g->n; i++)
        {
            encode_stair(i, w);
        }

        for (auto edge : g->edges)
        {
            glue_stair(edge.first - 1, edge.second - 1, w);
        }

        if (is_debug_mode)
        {
            std::cout << "OBJ-K aux vars:\n";
            for (auto var : obj_k_aux_vars)
            {
                std::cout << "(" << var.first.first << ", " << var.first.second << ") - " << var.second << std::endl;
            }
        }
    }

    void DuplexNSCEncoder::encode_stair(int stair, unsigned w)
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
    }

    /*
     * Encode each window seperately.
     * The first window only has lower part.
     * The last window only has upper part.
     * Other windows have both upper part and lower part.
     */
    void DuplexNSCEncoder::encode_window(int window, int stair, unsigned w)
    {
        if (window == 0)
        {
            // Encode the first window, which only have lower part
            int lastVar = stair * (int)g->n + window * (int)w + w;

            for (int i = w; i > 1; i--)
            {
                int var = stair * (int)g->n + window * (int)w + i;
                cv->add_clause({-var, get_obj_k_aux_var(var, lastVar)});
            }

            for (int i = w; i > 2; i--)
            {
                int var = stair * (int)g->n + window * (int)w + i;
                cv->add_clause({-get_obj_k_aux_var(var, lastVar), get_obj_k_aux_var(var - 1, lastVar)});
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
                for (int i = 1; i <= real_w; i++)
                {
                    int reverse_var = stair * (int)g->n + window * (int)w + i;
                    cv->add_clause({-reverse_var, get_obj_k_aux_var(firstVar, reverse_var)});
                }

                for (int i = real_w - 1; i > 0; i--)
                {
                    int reverse_var = stair * (int)g->n + window * (int)w + real_w - i;
                    cv->add_clause({-get_obj_k_aux_var(firstVar, reverse_var), get_obj_k_aux_var(firstVar, reverse_var + 1)});
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
                for (int i = 1; i < (int)w; i++)
                {
                    int reverse_var = stair * (int)g->n + window * (int)w + i;
                    cv->add_clause({-reverse_var, get_obj_k_aux_var(firstVar, reverse_var)});
                }

                for (int i = w - 1; i > 1; i--)
                {
                    int reverse_var = stair * (int)g->n + window * (int)w + w - i;
                    cv->add_clause({-get_obj_k_aux_var(firstVar, reverse_var), get_obj_k_aux_var(firstVar, reverse_var + 1)});
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
            for (int i = 1; i < (int)w; i++)
            {
                int reverse_var = stair * (int)g->n + window * (int)w + i;
                cv->add_clause({-reverse_var, get_obj_k_aux_var(firstVar, reverse_var)});
            }

            for (int i = w - 1; i > 1; i--)
            {
                int reverse_var = stair * (int)g->n + window * (int)w + w - i;
                cv->add_clause({-get_obj_k_aux_var(firstVar, reverse_var), get_obj_k_aux_var(firstVar, reverse_var + 1)});
            }

            for (int i = (int)w; i > 1; i--)
            {
                int reverse_var = stair * (int)g->n + window * (int)w + i;
                cv->add_clause({-reverse_var, -get_obj_k_aux_var(firstVar, reverse_var - 1)});
            }

            // Lower part
            int lastVar = stair * (int)g->n + window * (int)w + w;
            for (int i = w; i > 1; i--)
            {
                int var = stair * (int)g->n + window * (int)w + i;
                cv->add_clause({-var, get_obj_k_aux_var(var, lastVar)});
            }

            for (int i = w; i > 2; i--)
            {
                int var = stair * (int)g->n + window * (int)w + i;
                cv->add_clause({-get_obj_k_aux_var(var, lastVar), get_obj_k_aux_var(var - 1, lastVar)});
            }

            // Can be disable
            // for (int i = 1; i < (int)w; i++)
            // {
            //     int var = stair * (int)g->n + window * (int)w + i;
            //     cv->add_clause({-var, -GetEncodedAuxVar(auxStartVarLP + var + 1)});
            // }
        }
    }

    /*
     * Glue adjacent windows with each other.
     * Using lower part of the previous window and upper part of the next window
     * as anchor points to glue.
     */
    void DuplexNSCEncoder::glue_window(int window, int stair, unsigned w)
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

    void DuplexNSCEncoder::glue_stair(int stair1, int stair2, unsigned w)
    {
        if (is_debug_mode)
            std::cout << "Glue stair " << stair1 << " with stair " << stair2 << std::endl;
        int number_step = g->n - w + 1;
        for (int i = 0; i < number_step; i++)
        {
            int mod = i % w;
            int subset = i / w;
            int firstVar, secondVar, thirdVar, forthVar;
            if (mod == 0)
            {
                if (i < number_step - 1)
                {
                    firstVar = stair1 * g->n + subset * w + 1;
                    secondVar = get_obj_k_aux_var(stair1 * g->n + subset * w + 2, stair1 * g->n + subset * w + w);
                    thirdVar = stair2 * g->n + subset * w + 1;
                    forthVar = get_obj_k_aux_var(stair2 * g->n + subset * w + 2, stair2 * g->n + subset * w + w);
                }
                else
                {
                    firstVar = stair1 * g->n + subset * w + w;
                    secondVar = get_obj_k_aux_var(stair1 * g->n + subset * w + 1, stair1 * g->n + subset * w + w - 1);
                    thirdVar = stair2 * g->n + subset * w + w;
                    forthVar = get_obj_k_aux_var(stair2 * g->n + subset * w + 1, stair2 * g->n + subset * w + w - 1);
                }
            }
            else
            {
                firstVar = get_obj_k_aux_var(stair1 * g->n + subset * w + 1 + mod, stair1 * g->n + subset * w + w);
                secondVar = get_obj_k_aux_var(stair1 * g->n + subset * w + w + 1, stair1 * g->n + subset * w + w + mod);
                thirdVar = get_obj_k_aux_var(stair2 * g->n + subset * w + 1 + mod, stair2 * g->n + subset * w + w);
                forthVar = get_obj_k_aux_var(stair2 * g->n + subset * w + w + 1, stair2 * g->n + subset * w + w + mod);
            }
            cv->add_clause({-firstVar, -thirdVar});
            cv->add_clause({-firstVar, -forthVar});
            cv->add_clause({-secondVar, -thirdVar});
            cv->add_clause({-secondVar, -forthVar});
        }
    }
}
