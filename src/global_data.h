#ifndef GLOBAL_DATA_H
#define GLOBAL_DATA_H

#include "graph/graph.h"
#include "enum/encode_type.h"
#include "enum/sat_solver_type.h"
#include "enum/search_strategy.h"
#include "enum/symmetry_breaking_type.h"
#include <unordered_map>
#include <limits>

class GlobalData
{
public:
    GlobalData();
    ~GlobalData();

    static Graph *g; // Pointer to the global graph instance

    static int worker_count;

    static EncodeType encode_type;
    static SearchStrategy search_strategy;
    static SymmetryBreakingType symmetry_break_strategy;
    static SATSolverType sat_solver_type;

    static bool just_print_dimacs;
    static std::string dimacs_directory;

    static bool enable_solution_verification;
    static int split_limit;

    static bool overwrite_lb;
    static bool overwrite_ub;
    static int forced_lb;
    static int forced_ub;

    static int sample_rate; // Interval of sampler, in microseconds
    static int report_rate; // Interval of report, in number of sampler

    static float memory_limit;       // bound of total memory consumed by all the processes, in megabyte
    static float real_time_limit;    // bound of time consumed by main process, in seconds
    static float elapsed_time_limit; // bound of total time consumed by all the process, in seconds

    static void read_graph(std::string graph_file_name);

    static std::unordered_map<std::string, int> abw_LBs;
    static std::unordered_map<std::string, int> abw_UBs;
};

#endif // GLOBAL_DATA_H