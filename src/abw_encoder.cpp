#include "global_data.h"
#include "abw_encoder.h"
#include "searchers/abw_searcher_iterate_from_lb.h"

#include <iostream>

AntibandwidthEncoder::AntibandwidthEncoder() {};

AntibandwidthEncoder::~AntibandwidthEncoder()
{
    if (abw_searcher != nullptr)
    {
        delete abw_searcher;
        abw_searcher = nullptr;
    }
};

void AntibandwidthEncoder::setup_searcher()
{
    switch (GlobalData::search_strategy)
    {
    case SearchStrategy::iterate_from_lb:
        std::cout << "c [Main] Search strategy: Iterating from lower bound.\n";
        abw_searcher = new ABWSearcherIterateFromLB();
        break;

    default:
        std::cerr << "e [Main] Unrecognized search strategy " << static_cast<int>(GlobalData::search_strategy) << ".\n";
        break;
    }
}

void AntibandwidthEncoder::encode_and_solve()
{
    std::cout << "c [Main] Encoding and solving for graph: " << GlobalData::g->graph_name << ".\n";

    setup_searcher();

    abw_searcher->encode_and_solve();
};

void AntibandwidthEncoder::encode_and_print_dimacs()
{
    std::cout << "c [Main] Encoding and printing DIMACS for graph: " << GlobalData::g->graph_name << ".\n";

    setup_searcher();

    abw_searcher->encode_and_print_dimacs();
};
