#include "antibandwidth_encoder.h"
#include "abp_encoder.h"

#include <iostream>
#include <assert.h>
#include <chrono>
#include <unistd.h>
#include <signal.h>

namespace SATABP
{
    int AntibandwidthEncoder::max_width_SAT = INT_MIN;
    int AntibandwidthEncoder::min_width_UNSAT = INT_MAX;

    AntibandwidthEncoder::AntibandwidthEncoder() {};

    AntibandwidthEncoder::~AntibandwidthEncoder() {};

    void AntibandwidthEncoder::read_graph(std::string graph_file_name)
    {
        graph = new Graph(graph_file_name);
    };

    void AntibandwidthEncoder::encode_and_solve_abws()
    {
        switch (iterative_strategy)
        {
        case from_lb:
            std::cout << "c Iterative strategy: from LB to UB.\n";
            encode_and_solve_abw_problems_from_lb();
            break;
        case from_ub:
            std::cout << "c Iterative strategy: from UB to LB.\n";
            encode_and_solve_abw_problems_from_ub();
            break;
        case bin_search:
            std::cout << "c Iterative strategy: binary search between LB and UB.\n";
            encode_and_solve_abw_problems_bin_search();
            break;
        default:
            std::cerr << "c Unrecognized iterative strategy " << iterative_strategy << ".\n";
            return;
        }
    };

    void AntibandwidthEncoder::encode_and_solve_abw_problems_from_lb()
    {
        int w_from, w_to;
        setup_bounds(w_from, w_to);
        encode_and_solve_abw_problems(w_from, +1, w_to + 1);
    };

    void AntibandwidthEncoder::encode_and_solve_abw_problems_from_ub()
    {
        std::cerr << "e Iterative strategy UB has not yet implemented.\n";
    };

    void AntibandwidthEncoder::encode_and_solve_abw_problems_bin_search()
    {
        std::cerr << "e Iterative strategy bin has not yet implemented.\n";
    };

    void AntibandwidthEncoder::do_child_pid_task(int width)
    {
        std::cout << "c [w = " << width << "] " << width << " starting." << std::endl;

        // Dynamically allocate and use ABPEncoder in child process
        ABPEncoder *abp_enc = new ABPEncoder(graph, width);
        int result = abp_enc->encode_and_solve_abp();

        std::cout << "c [w = " << width << "] Result: " << result << "\n";

        // Clean up dynamically allocated memory
        delete abp_enc;

        // Handle results as in the original logic
        switch (result)
        {
        case 0:
            std::cout << "c [w = " << width << "] Graph is valid with any width.\n";
            break;
        case 10:
            if (width > max_width_SAT)
            {
                std::cout << "c [w = " << width << "] Max width SAT is set to " << width << "\n";
                max_width_SAT = width;
            }

            for (auto it = child_pids.begin(); it != child_pids.end(); ++it)
            {
                if (it->first < width)
                {
                    kill(it->second, SIGTERM);
                }
            }
            break;
        case 20:
            if (width < min_width_UNSAT)
            {
                std::cout << "c [w = " << width << "] Min width UNSAT is set to " << width << "\n";
                min_width_UNSAT = width;
            }

            std::cout << "d [w = " << width << "] Child pids coutn: " << child_pids.size() << "\n";

            for (auto it = child_pids.begin(); it != child_pids.end(); ++it)
            {
                std::cout << "d [w = " << width << "] Travel child " << it->first << " - " << width << "\n";
                // if (it->first > width)
                // {
                //     kill(it->second, SIGTERM);
                // }
            }
            break;
        case -10:
            std::cout << "e [w = " << width << "] Get an incorrect answer.\n";
            break;
        case -20:
            std::cout << "e [w = " << width << "] Get an invalid answer.\n";
            break;
        default:
            break;
        }

        std::cout << "c [w = " << width << "] Child " << width << " completed task." << std::endl;
    }

    void AntibandwidthEncoder::encode_and_solve_abw_problems(int start_w, int step, int stop_w)
    {
        int current_width = start_w;
        int number_width = stop_w - start_w;

        for (int i = 0; i < thread_count && i < number_width; i += step)
        {
            pid_t pid = fork();

            if (pid < 0)
            {
                std::cerr << "c [w = " << current_width << "] Fork failed!\n";
            }
            else if (pid == 0)
            {
                sleep(1);

                // Child process: perform the task
                do_child_pid_task(current_width);

                exit(0);
            }
            else
            {
                // Parent process stores the child's PID
                child_pids[current_width] = pid;

                std::cout << "c [w = " << current_width << "] Fork failed!\n";
            }

            current_width += step;
        }

        // Parent process waits until all child processes finish
        while (!child_pids.empty())
        {
            int status;
            pid_t finished_pid = wait(&status); // Wait for any child to complete

            if (finished_pid != -1)
            {
                // Remove the finished child from the map
                for (auto it = child_pids.begin(); it != child_pids.end(); ++it)
                {
                    if (it->second == finished_pid)
                    {
                        std::cout << "Child pid " << it->first << " - " << finished_pid << " ends with status " << status << "\n";
                        child_pids.erase(it);
                        break;
                    }
                }

                std::string log_remaining_child_pids = "Remaining child pids:\n";
                for (auto it = child_pids.begin(); it != child_pids.end(); ++it)
                {
                    log_remaining_child_pids.append("\tPid ");
                    log_remaining_child_pids.append(std::to_string(it->first));
                    log_remaining_child_pids.append(" - ");
                    log_remaining_child_pids.append(std::to_string(it->second));
                    log_remaining_child_pids.append("\n");
                }
                std::cout << log_remaining_child_pids;
            }
        }

        std::cout << "Parent: All children have completed their tasks or were terminated." << std::endl;
    };

    void AntibandwidthEncoder::encode_and_print_abw_problem(int w)
    {
        w = w;
        std::cerr << "e Encode and print ABP have not yet implemented.\n";
    };

    void AntibandwidthEncoder::setup_bounds(int &w_from, int &w_to)
    {
        lookup_bounds(w_from, w_to);

        if (overwrite_lb)
        {
            std::cout << "c LB " << w_from << " is overwritten with " << forced_lb << ".\n";
            w_from = forced_lb;
        }
        if (overwrite_ub)
        {
            std::cout << "c UB " << w_to << " is overwritten with " << forced_ub << ".\n";
            w_to = forced_ub;
        }
        if (w_from > w_to)
        {
            int tmp = w_from;
            w_from = w_to;
            w_to = tmp;
            std::cout << "c Flipped LB and UB to avoid LB > UB: (LB = " << w_from << ", UB = " << w_to << ").\n";
        }

        assert((w_from <= w_to) && (w_from >= 1));
    };

    void AntibandwidthEncoder::lookup_bounds(int &lb, int &ub)
    {
        auto pos = abw_LBs.find(graph->graph_name);
        if (pos != abw_LBs.end())
        {
            lb = pos->second;
            std::cout << "c LB-w = " << lb << " (LB in Sinnl - A note on computational approaches for the antibandwidth problem).\n";
        }
        else
        {
            lb = 1;
            std::cout << "c No predefined lower bound is found for " << graph->graph_name << ".\n";
            std::cout << "c LB-w = 1 (default value).\n";
        }

        pos = abw_UBs.find(graph->graph_name);
        if (pos != abw_UBs.end())
        {
            ub = pos->second;
            if (verbose)
                std::cout << "c UB-w = " << ub << " (UB in Sinnl - A note on computational approaches for the antibandwidth problem).\n";
        }
        else
        {
            ub = graph->n / 2 + 1;
            std::cout << "c No predefined upper bound is found for " << graph->graph_name << ".\n";
            std::cout << "c UB-w = " << ub << " (default value calculated as n/2+1).\n";
        }
    };

    std::unordered_map<std::string, int> AntibandwidthEncoder::abw_LBs = {
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

    std::unordered_map<std::string, int> AntibandwidthEncoder::abw_UBs = {
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
}
