#include "global_data.h"

GlobalData::GlobalData() {}

GlobalData::~GlobalData()
{
    if (g)
        delete g; // Clean up the global graph instance
}

Graph *GlobalData::g = nullptr;

int GlobalData::worker_count = 1;

EncodeType GlobalData::encode_type = EncodeType::scl;
SearchStrategy GlobalData::search_strategy = SearchStrategy::iterate_from_lb;
SymmetryBreakingType GlobalData::symmetry_break_strategy = SymmetryBreakingType::NONE;
SATSolverType GlobalData::sat_solver_type = SATSolverType::CaDiCaL;

bool GlobalData::just_print_dimacs = false;
std::string GlobalData::dimacs_directory = "./dimacs_output/";

bool GlobalData::enable_solution_verification = true;
int GlobalData::split_limit = 0;

bool GlobalData::overwrite_lb = false;
bool GlobalData::overwrite_ub = false;
int GlobalData::forced_lb = 0;
int GlobalData::forced_ub = 0;

int GlobalData::sample_rate = 100000;
int GlobalData::report_rate = 100;

float GlobalData::memory_limit = std::numeric_limits<float>::max();
float GlobalData::real_time_limit = std::numeric_limits<float>::max();
float GlobalData::elapsed_time_limit = std::numeric_limits<float>::max();

void GlobalData::read_graph(std::string graph_file_name)
{
    g = new Graph(graph_file_name);
};

std::unordered_map<std::string, int> GlobalData::abw_LBs = {
    {"A-pores_1.mtx.rnd", 6},
    {"B-ibm32.mtx.rnd", 9},
    {"C-bcspwr01.mtx.rnd", 16},
    {"D-bcsstk01.mtx.rnd", 8},
    {"E-bcspwr02.mtx.rnd", 21},
    {"F-curtis54.mtx.rnd", 12},
    {"G-will57.mtx.rnd", 12},
    {"H-impcol_b.mtx.rnd", 8},
    {"I-ash85.mtx.rnd", 19},
    {"J-nos4.mtx.rnd", 32},
    {"K-dwt__234.mtx.rnd", 46},
    {"L-bcspwr03.mtx.rnd", 39},
    {"M-bcsstk06.mtx.rnd", 28},
    {"N-bcsstk07.mtx.rnd", 28},
    {"O-impcol_d.mtx.rnd", 91},
    {"P-can__445.mtx.rnd", 78},
    {"Q-494_bus.mtx.rnd", 219},
    {"R-dwt__503.mtx.rnd", 46},
    {"S-sherman4.mtx.rnd", 256},
    {"T-dwt__592.mtx.rnd", 103},
    {"U-662_bus.mtx.rnd", 219},
    {"V-nos6.mtx.rnd", 326},
    {"W-685_bus.mtx.rnd", 136},
    {"X-can__715.mtx.rnd", 112}};

std::unordered_map<std::string, int> GlobalData::abw_UBs = {
    {"A-pores_1.mtx.rnd", 8},
    {"B-ibm32.mtx.rnd", 9},
    {"C-bcspwr01.mtx.rnd", 17},
    {"D-bcsstk01.mtx.rnd", 9},
    {"E-bcspwr02.mtx.rnd", 22},
    {"F-curtis54.mtx.rnd", 13},
    {"G-will57.mtx.rnd", 14},
    {"H-impcol_b.mtx.rnd", 8},
    {"I-ash85.mtx.rnd", 27},
    {"J-nos4.mtx.rnd", 40},
    {"K-dwt__234.mtx.rnd", 58},
    {"L-bcspwr03.mtx.rnd", 39},
    {"M-bcsstk06.mtx.rnd", 72},
    {"N-bcsstk07.mtx.rnd", 72},
    {"O-impcol_d.mtx.rnd", 173},
    {"P-can__445.mtx.rnd", 120},
    {"Q-494_bus.mtx.rnd", 246},
    {"R-dwt__503.mtx.rnd", 71},
    {"S-sherman4.mtx.rnd", 272},
    {"T-dwt__592.mtx.rnd", 150},
    {"U-662_bus.mtx.rnd", 220},
    {"V-nos6.mtx.rnd", 337},
    {"W-685_bus.mtx.rnd", 136},
    {"X-can__715.mtx.rnd", 142}};