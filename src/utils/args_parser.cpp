#include "args_parser.h"
#include "../global_data.h"
#include "iostream"
#include "stdexcept"
#include "usage.h"

std::unordered_map<std::string, Command> ArgsParser::cmd;

void ArgsParser::init_parser()
{
    cmd["--duplex"] = [](int &, int, char **)
    {
        GlobalData::encode_type = EncodeType::duplex;
    };

    cmd["--reduced"] = [](int &, int, char **)
    {
        GlobalData::encode_type = EncodeType::reduced;
    };

    cmd["--product"] = [](int &, int, char **)
    {
        GlobalData::encode_type = EncodeType::product;
    };

    cmd["--scl"] = [](int &, int, char **)
    {
        GlobalData::encode_type = EncodeType::scl;
    };

    cmd["--seq"] = [](int &, int, char **)
    {
        GlobalData::encode_type = EncodeType::seq;
    };

    cmd["--verify-result"] = [](int &, int, char **)
    {
        GlobalData::enable_solution_verification = true;
    };

    cmd["--iterate-from-lb"] = [](int &, int, char **)
    {
        GlobalData::search_strategy = SearchStrategy::iterate_from_lb;
    };

    cmd["-set-lb"] = [](int &i, int argc, char **argv)
    {
        int val = get_positive(i, argc, argv, "lower bound");

        if (val < 2)
            throw std::runtime_error("Lower bound has to be at least 2");
        if (val > GlobalData::g->n / 2)
            throw std::runtime_error("Lower bound cannot be greater than n/2");

        GlobalData::forced_lb = val;
        GlobalData::overwrite_lb = true;

        std::cout << "c [Param] LB is predefined as " << val << ".\n";
    };

    cmd["-set-ub"] = [](int &i, int argc, char **argv)
    {
        int val = get_positive(i, argc, argv, "upper bound");

        if (val < 2)
            throw std::runtime_error("Upper bound has to be at least 2");
        if (val > GlobalData::g->n / 2)
            throw std::runtime_error("Upper bound cannot be greater than n/2");

        GlobalData::forced_ub = val;
        GlobalData::overwrite_ub = true;

        std::cout << "c [Param] UB is predefined as " << val << ".\n";
    };

    cmd["-limit-memory"] = [](int &i, int argc, char **argv)
    {
        int v = get_positive(i, argc, argv, "memory limit");
        GlobalData::memory_limit = v;
        std::cout << "c [Param] Memory limit is set to " << v << ".\n";
    };

    cmd["-limit-real-time"] = [](int &i, int argc, char **argv)
    {
        int v = get_positive(i, argc, argv, "real time limit");
        GlobalData::real_time_limit = v;
        std::cout << "c [Param] Real time limit is set to " << v << ".\n";
    };

    cmd["-limit-elapsed-time"] = [](int &i, int argc, char **argv)
    {
        int v = get_positive(i, argc, argv, "elapsed time limit");
        GlobalData::elapsed_time_limit = v;
        std::cout << "c [Param] Elapsed time limit is set to " << v << ".\n";
    };

    cmd["-sample-rate"] = [](int &i, int argc, char **argv)
    {
        int v = get_positive(i, argc, argv, "sample rate");
        GlobalData::sample_rate = v;
        std::cout << "c [Param] Sample rate is set to " << v << ".\n";
    };

    cmd["-report-rate"] = [](int &i, int argc, char **argv)
    {
        int v = get_positive(i, argc, argv, "report rate");
        GlobalData::report_rate = v;
        std::cout << "c [Param] Report rate is set to " << v << ".\n";
    };

    cmd["-split-size"] = [](int &i, int argc, char **argv)
    {
        int v = get_positive(i, argc, argv, "split size");
        GlobalData::split_limit = v;
        std::cout << "c [Param] Splitting clauses at length " << v << ".\n";
    };

    cmd["-worker-count"] = [](int &i, int argc, char **argv)
    {
        int v = get_positive(i, argc, argv, "worker count");
        GlobalData::worker_count = v;
        std::cout << "c [Param] Worker count is set to " << v << ".\n";
    };

    cmd["-symmetry-break"] = [](int &i, int argc, char **argv)
    {
        if (i + 1 >= argc)
            throw std::runtime_error("Missing symmetry type");

        std::string s = argv[++i];

        if (s == "none")
            GlobalData::symmetry_break_strategy = SymmetryBreakingType::NONE;
        else if (s == "first")
            GlobalData::symmetry_break_strategy = SymmetryBreakingType::FIRST;
        else if (s == "highest-degree")
            GlobalData::symmetry_break_strategy = SymmetryBreakingType::HIGHEST_DEGREE;
        else if (s == "lowest-degree")
            GlobalData::symmetry_break_strategy = SymmetryBreakingType::LOWEST_DEGREE;
        else
            throw std::runtime_error("Unrecognized symmetry breaking type: " + s);
    };

    cmd["-sat-solver"] = [](int &i, int argc, char **argv)
    {
        if (i + 1 >= argc)
            throw std::runtime_error("Missing solver type");

        std::string s = argv[++i];

        if (s == "cadical")
            GlobalData::sat_solver_type = SATSolverType::CaDiCaL;
        else
            throw std::runtime_error("Unrecognized SAT solver type: " + s);
    };

    cmd["-just-print-dimacs"] = [](int &i, int argc, char **argv)
    {
        if (i + 1 >= argc)
            throw std::runtime_error("Missing directory");

        GlobalData::just_print_dimacs = true;
        GlobalData::dimacs_directory = argv[++i];
    };
}

int ArgsParser::try_parse_args(int argc, char **argv)
{
    try
    {
        for (int i = 1; i < argc; i++)
        {
            std::string arg = argv[i];

            if (arg[0] != '-')
            {
                GlobalData::read_graph(arg);
                continue;
            }

            auto it = cmd.find(arg);
            if (it != cmd.end())
            {
                it->second(i, argc, argv);
            }
            else if (arg == "--help")
            {
                Helper::print_usage();
                return 1;
            }
            else
            {
                throw std::runtime_error("Unrecognized option: " + arg);
            }
        }
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "e [Param] " << e.what() << "\n";
        return -1;
    }
}

int ArgsParser::get_int(const std::string &arg)
{
    try
    {
        return std::stoi(arg);
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error("Invalid number: " + arg);
    }
}

int ArgsParser::get_positive(int &i, int argc, char **argv, const std::string &name)
{
    if (i + 1 >= argc)
        throw std::runtime_error("Missing value for " + name);
    int val = get_int(argv[++i]);
    if (val <= 0)
        throw std::runtime_error(name + " must be positive");
    return val;
}
