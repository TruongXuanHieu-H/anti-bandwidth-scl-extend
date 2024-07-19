#ifndef DUPLEX_NSC_ENCODER
#define DUPLEX_NSC_ENCODER

#include "encoder.h"

namespace SATABP
{

    class DuplexNSCEncoder : public Encoder
    {
    public:
        DuplexNSCEncoder(Graph *g, ClauseContainer *cc, VarHandler *vh);
        virtual ~DuplexNSCEncoder();

    private:
        void do_encode_antibandwidth(unsigned w, const std::vector<std::pair<int, int>> &node_pairs) final;

        int do_vars_size() const final;

        int get_aux_var(int symbolicAuxVar);
        int get_obj_k_aux_var(int first, int last);

        void encode_vertices();
        void encode_labels();
        void encode_exactly_one(std::vector<int> listVars, int auxVar);

        void encode_obj_k(unsigned w);
        void encode_stair(int stair, unsigned w);
        void encode_window(int window, int stair, unsigned w);
        void glue_window(int window, int stair, unsigned w);
        void glue_stair(int stair1, int stair2, unsigned w);
    };
}

#endif