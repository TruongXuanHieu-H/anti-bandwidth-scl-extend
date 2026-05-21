#include <map>
#include <string>
#include <iostream>
#include <iomanip>

#include "usage.h"

const std::map<std::string, std::string> Helper::option_list = {
    {"--help", "Print usage message with all possible options"},
    {"--reduced", "Use reduced naive encoding for staircase constraints [default: false]"},
    {"--seq", "Use sequential encoding for staircase constraints [default: false]"},
    {"--product", "Use 2-Product encoding for staircase constraints [default: false]"},
    {"--duplex", "Use duplex encoding for staircase constraints [default: true]"},
    {"--scl", "Use scl encoding for staircase constraints and NSC for At-Most-One constraints [default: false]"},
    {"--conf-sat", "Use --sat configuration of CaDiCaL [default: true]"},
    {"--conf-unsat", "Use --unsat configuration of CaDiCaL [default: false]"},
    {"--force-phase", "Set options --forcephase,--phase=0 and --no-rephase of CaDiCal [default: false]"},
    {"--verify-result", "Verify the antibandwidth of the found SAT solution [default: false]"},
    {"--from-lb", "Start solving with width = LB, increasing in each iteration [default: true]"},
    {"-split-size <n>", "Maximal allowed length of clauses, every longer clause is split up into two by introducing a new variable"},
    {"-set-lb <new LB>", "Overwrite predefined LB with <new LB>, has to be at least 2"},
    {"-set-ub <new UB>", "Overwrite predefined UB with <new UB>, has to be positive"},
    {"-limit-memory <MB>", "Overwrite memory consumption restriction [default: FLOAT_MAX]"},
    {"-limit-real-time <seconds>", "Overwrite real time consumption restriction [default: FLOAT_MAX]"},
    {"-limit-elapsed-time <seconds>", "Overwrite elapsed time consumption restriction [default: FLOAT_MAX]"},
    {"-sample-rate <microseconds>", "Overwrite sampler interval [default: 100000]"},
    {"-report-rate <sampler>", "Overwrite elapsed time interval [default: 100 samplers generates 1 report]"},
    {"-symmetry-break <break point>", "Apply symetry breaking technique in <break point> (f: first node, h: highest degree node, l: lowest degree node, n: none) [default: none]"},
    {"-process-count <number process>", "Number processes used to solve"}};

void Helper::print_usage()
{
    std::cout << "c [Usage] Usage: cabw_enc path_to_graph_file [ <option> ... ].\n";
    std::cout << "c [Usage] where '<option>' is one of the following options:\n";
    std::cout << std::endl;
    for (const auto &option : option_list)
    {
        std::cout << std::left << "\t" << std::setw(30) << option.first << "\t" << option.second << ".\n";
    }
    std::cout << std::endl;
}