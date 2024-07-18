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

        /*
         *  Get actual aux var to push into solver from aux var deducted from encoding method
         */
        int GetEncodedAuxVar(int symbolicAuxVar);

        /*
         *  Get number of aux var requires to encode a single stair
         */
        int GetAuxVarPerStair(unsigned w);

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