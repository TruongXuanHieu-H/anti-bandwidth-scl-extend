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
    bool isDebugMode = true;

    int verticesAuxVar = 0;
    int labelsAuxVar = 0;
    int objKAuxVar = 0;
    int auxVarPerStair = 0;
    std::map<int, int> auxVarConverter = {};
    std::map<std::pair<int, int>, int> registerBits = {};

    DuplexNSCEncoder::DuplexNSCEncoder(Graph *g, ClauseContainer *cc, VarHandler *vh) : Encoder(g, cc, vh)
    {
    }

    DuplexNSCEncoder::~DuplexNSCEncoder() {}

    int DuplexNSCEncoder::GetEncodedAuxVar(int symbolicAuxVar)
    {
        auto pair = auxVarConverter.find(symbolicAuxVar);

        if (pair != auxVarConverter.end())
        {
            // Key found, return the value
            // std::cout << "Query key " << symbolicAuxVar << " and get " << pair->second << std::endl;
            return pair->second;
        }
        else
        {
            // Key not found, create and add key then return the value
            int encoderAuxVar = vh->get_new_var();
            auxVarConverter.insert({symbolicAuxVar, encoderAuxVar});
            // std::cout << "Insert key " << symbolicAuxVar << " with value " << encoderAuxVar << std::endl;
            return encoderAuxVar;
        }
    }

    int DuplexNSCEncoder::do_vars_size() const
    {
        return vh->size();
    };

    void DuplexNSCEncoder::do_encode_antibandwidth(unsigned w, const std::vector<std::pair<int, int>> &node_pairs)
    {
        verticesAuxVar = g->n * g->n;
        labelsAuxVar = verticesAuxVar + g->n * g->n;
        objKAuxVar = labelsAuxVar + g->n * g->n;
        auxVarPerStair = GetAuxVarPerStair(w);
        auxVarConverter.clear();
        registerBits.clear();
        encode_vertices();
        encode_labels();
        encode_obj_k(w);

        // Prevent error when build due to unused variables
        (void)node_pairs;
    };

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
     */
    int DuplexNSCEncoder::GetAuxVarPerStair(unsigned w)
    {
        int result = 0;
        // Because all windows can't be longer than w, we assume all of them have width of w,
        // which requires the highest number of aux var to encode.

        // Although the first and the last windows need w aux vars each
        // because they only have upper or lower part, we still count as if
        // they have both upper part and lower part in order to identify which
        // part the aux var belong to;
        // result += 2 * w; // The first window
        // result += 2 * w; // The last window

        // The middle windows need 2 * w aux vars each
        // int numberMiddleWindows = ceil((float)g->n / w) - 2;
        // result += numberMiddleWindows * 2 * w;

        // In total, number aux var per stair is equal number windows * 2 * w
        result = ceil((float)g->n / w) * 2 * w;

        return result;
    }

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
            encode_exactly_one(node_vertices_eo, verticesAuxVar + i * g->n);
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
            encode_exactly_one(node_labels_eo, labelsAuxVar + i * g->n);
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
            cv->add_clause({-listVars[i - 1], GetEncodedAuxVar(auxVar + i)});
        }

        // Constraint 2: R(i-1, 1) -> R(i, 1) for i in [2, n - 1]
        // In CNF: not(R(i-1, 1)) or R(i, 1)
        for (int i = 2; i <= listVarsSize - 1; i++)
        {
            cv->add_clause({-(GetEncodedAuxVar(auxVar + i - 1)), GetEncodedAuxVar(auxVar + i)});
        }

        // Constraint 3: Since k = 1 (Exactly 1 constraint), this constraint is empty and then skipped.

        // Constraint 4: not(Xi) and not(R(i-1, 1)) -> not(R(i, 1)) for i in [2, n - 1]
        // In CNF: Xi or R(i-1, 1) or not (R(i, 1))
        for (int i = 2; i <= listVarsSize - 1; i++)
        {
            cv->add_clause({listVars[i - 1], GetEncodedAuxVar(auxVar + i - 1), -(GetEncodedAuxVar(auxVar + i))});
        }

        // Constraint 5: not(X1) -> not(R(1,1))
        // In CNF: X1 or not(R(1,1))
        cv->add_clause({listVars[0], -(GetEncodedAuxVar(auxVar + 1))});

        // Constraint 6: Since k = 1 (Exactly 1 constraint), this constraint is empty and then skipped.

        // Constraint 7: (At Least k) R(n-1, 1) or Xn
        // In CNF: R(n-1, 1) or Xn
        cv->add_clause({GetEncodedAuxVar(auxVar + listVarsSize - 1), listVars[listVarsSize - 1]});

        // Constraint 8: (At Most k) Xi -> not(R(i-1,1)) for i in [k + 1, n]
        // In CNF: not(Xi) or not(R(i-1,1))
        for (int i = 2; i <= listVarsSize; i++)
        {
            cv->add_clause({-listVars[i - 1], -(GetEncodedAuxVar(auxVar + i - 1))});
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

        std::cout << "Register Bits: \n";
        for (auto registerBit : registerBits)
        {
            std::cout << "(" << registerBit.first.first << ", " << registerBit.first.second << ") - " << registerBit.second << std::endl;
        }
    }

    void DuplexNSCEncoder::encode_stair(int stair, unsigned w)
    {
        if (isDebugMode)
            std::cout << "Encode stair " << stair << " with width " << w << std::endl;

        for (int gw = 0; gw < ceil((float)g->n / w); gw++)
        {
            if (isDebugMode)
                std::cout << "Encode window " << gw << std::endl;
            encode_window(gw, stair, w);
        }

        for (int gw = 0; gw < ceil((float)g->n / w) - 1; gw++)
        {
            if (isDebugMode)
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
        int auxStartVarLP = objKAuxVar + stair * auxVarPerStair + auxVarPerStair / 2;
        int auxStartVarUP = objKAuxVar + stair * auxVarPerStair;
        if (window == 0)
        {
            // Encode the first window, which only have lower part
            for (int i = w; i > 1; i--)
            {
                int var = stair * (int)g->n + window * (int)w + i;
                int auxVar = GetEncodedAuxVar(auxStartVarLP + var);
                registerBits.insert({{var, var + w - i}, auxVar});
                if (isDebugMode)
                    std::cout << "Insert 1 (" << var << ", " << var + w - i << ") - " << auxVar << std::endl;
                cv->add_clause({-var, auxVar});
            }

            for (int i = w; i > 2; i--)
            {
                int var = stair * (int)g->n + window * (int)w + i;
                cv->add_clause({-GetEncodedAuxVar(auxStartVarLP + var), GetEncodedAuxVar(auxStartVarLP + var - 1)});
            }

            for (int i = 1; i < (int)w; i++)
            {
                int var = stair * (int)g->n + window * (int)w + i;
                cv->add_clause({-var, -GetEncodedAuxVar(auxStartVarLP + var + 1)});
            }
        }
        else if (window == ceil((float)g->n / w) - 1)
        {
            // Encode the last window, which only have upper part and may have width lower than w
            if ((window + 1) * w > g->n)
            {
                int real_w = g->n % w;
                // Upper part
                for (int i = 1; i <= real_w; i++)
                {
                    int reverse_var = stair * (int)g->n + window * (int)w + i;
                    int reverse_auxVar = GetEncodedAuxVar(auxStartVarUP + reverse_var);
                    registerBits.insert({{reverse_var + 1 - i, reverse_var}, reverse_auxVar});
                    if (isDebugMode)
                        std::cout << "Insert 2 (" << reverse_var + 1 - i << ", " << reverse_var << ") - " << reverse_auxVar << std::endl;
                    cv->add_clause({-reverse_var, reverse_auxVar});
                }

                for (int i = real_w; i > 1; i--)
                {
                    int reverse_var = stair * (int)g->n + window * (int)w + w - i;
                    cv->add_clause({-GetEncodedAuxVar(auxStartVarUP + reverse_var), GetEncodedAuxVar(auxStartVarUP + reverse_var + 1)});
                }

                for (int i = real_w; i > 1; i--)
                {
                    int reverse_var = stair * (int)g->n + window * (int)w + i;
                    cv->add_clause({-reverse_var, -GetEncodedAuxVar(auxStartVarUP + reverse_var - 1)});
                }
            }
            else
            {
                // Upper part
                for (int i = 1; i < (int)w; i++)
                {
                    int reverse_var = stair * (int)g->n + window * (int)w + i;
                    int reverse_auxVar = GetEncodedAuxVar(auxStartVarUP + reverse_var);
                    registerBits.insert({{reverse_var + 1 - i, reverse_var}, reverse_auxVar});
                    if (isDebugMode)
                        std::cout << "Insert 3 (" << reverse_var + 1 - i << ", " << reverse_var << ") - " << reverse_auxVar << std::endl;
                    cv->add_clause({-reverse_var, reverse_auxVar});
                }

                for (int i = w - 1; i > 1; i--)
                {
                    int reverse_var = stair * (int)g->n + window * (int)w + w - i;
                    cv->add_clause({-GetEncodedAuxVar(auxStartVarUP + reverse_var), GetEncodedAuxVar(auxStartVarUP + reverse_var + 1)});
                }

                for (int i = (int)w; i > 1; i--)
                {
                    int reverse_var = stair * (int)g->n + window * (int)w + i;
                    cv->add_clause({-reverse_var, -GetEncodedAuxVar(auxStartVarUP + reverse_var - 1)});
                }
            }
        }
        else
        {
            // Encode the middle windows, which have both upper and lower path, and always have width w

            // Upper part
            for (int i = 1; i < (int)w; i++)
            {
                int reverse_var = stair * (int)g->n + window * (int)w + i;
                int reverse_auxVar = GetEncodedAuxVar(auxStartVarUP + reverse_var);
                registerBits.insert({{reverse_var + 1 - i, reverse_var}, reverse_auxVar});
                if (isDebugMode)
                    std::cout << "Insert 4 (" << reverse_var + 1 - i << ", " << reverse_var << ") - " << reverse_auxVar << std::endl;
                cv->add_clause({-reverse_var, reverse_auxVar});
            }

            for (int i = w - 1; i > 1; i--)
            {
                int reverse_var = stair * (int)g->n + window * (int)w + w - i;
                cv->add_clause({-GetEncodedAuxVar(auxStartVarUP + reverse_var), GetEncodedAuxVar(auxStartVarUP + reverse_var + 1)});
            }

            for (int i = (int)w; i > 1; i--)
            {
                int reverse_var = stair * (int)g->n + window * (int)w + i;
                cv->add_clause({-reverse_var, -GetEncodedAuxVar(auxStartVarUP + reverse_var - 1)});
            }

            // Lower part
            for (int i = w; i > 1; i--)
            {
                int var = stair * (int)g->n + window * (int)w + i;
                int auxVar = GetEncodedAuxVar(auxStartVarLP + var);
                registerBits.insert({{var, var + w - i}, auxVar});
                if (isDebugMode)
                    std::cout << "Insert 5 (" << var << ", " << var + w - i << ") - " << auxVar << std::endl;
                cv->add_clause({-var, auxVar});
            }

            for (int i = w; i > 2; i--)
            {
                int var = stair * (int)g->n + window * (int)w + i;
                cv->add_clause({-GetEncodedAuxVar(auxStartVarLP + var), GetEncodedAuxVar(auxStartVarLP + var - 1)});
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
        int auxStartVarLP = objKAuxVar + stair * auxVarPerStair + auxVarPerStair / 2;
        int auxStartVarUP = objKAuxVar + stair * auxVarPerStair;

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
        int real_w = w - 1;
        if ((window + 1) * w > g->n)
            real_w = g->n % w;

        for (int i = 1; i <= real_w; i++)
        {
            int reverse_var = stair * (int)g->n + (window + 1) * (int)w + real_w - i + 1;
            int var = stair * (int)g->n + window * (int)w + w - i + 1;

            cv->add_clause({-GetEncodedAuxVar(auxStartVarLP + var), -GetEncodedAuxVar(auxStartVarUP + reverse_var)});
        }
    }

    void DuplexNSCEncoder::glue_stair(int stair1, int stair2, unsigned w)
    {
        if (isDebugMode)
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
                    std::cout << "test 1 - " << i << "\n";
                    firstVar = stair1 * g->n + subset * w + 1;
                    secondVar = registerBits[{stair1 * g->n + subset * w + 2, stair1 * g->n + subset * w + w}];
                    thirdVar = stair2 * g->n + subset * w + 1;
                    forthVar = registerBits[{stair2 * g->n + subset * w + 2, stair2 * g->n + subset * w + w}];
                }
                else
                {

                    std::cout << "test 2 - " << i << "\n";
                    firstVar = stair1 * g->n + subset * w + w;
                    secondVar = registerBits[{stair1 * g->n + subset * w + 1, stair1 * g->n + subset * w + w - 1}];
                    thirdVar = stair2 * g->n + subset * w + w;
                    forthVar = registerBits[{stair2 * g->n + subset * w + 1, stair2 * g->n + subset * w + w - 1}];
                }
            }
            else
            {
                std::cout << "test 3 - " << i << "\n";
                firstVar = registerBits[{stair1 * g->n + subset * w + 1 + mod, stair1 * g->n + subset * w + w}];
                secondVar = registerBits[{stair1 * g->n + subset * w + w + 1, stair1 * g->n + subset * w + w + mod}];
                thirdVar = registerBits[{stair2 * g->n + subset * w + 1 + mod, stair2 * g->n + subset * w + w}];
                forthVar = registerBits[{stair2 * g->n + subset * w + w + 1, stair2 * g->n + subset * w + w + mod}];
            }
            cv->add_clause({-firstVar, -thirdVar});
            cv->add_clause({-firstVar, -forthVar});
            cv->add_clause({-secondVar, -thirdVar});
            cv->add_clause({-secondVar, -forthVar});
        }
    }
}
