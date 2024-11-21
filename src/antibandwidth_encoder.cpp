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
    std::unordered_map<int, pthread_t> AntibandwidthEncoder::threads;

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

    void *AntibandwidthEncoder::thread_function(void *args)
    {
        auto *params = static_cast<ThreadParams *>(args);
        AntibandwidthEncoder *abw_enc = params->abw_enc;
        int current_width = params->current_width;

        ABPEncoder *abp_enc = new ABPEncoder(abw_enc->graph, current_width);
        int result = abp_enc->encode_and_solve_abp();
        delete abp_enc;

        // Handle results as in the original logic
        switch (result)
        {
        case 0:
            std::cout << "c Graph is valid with any width.\n";
            break;
        case 10:
            if (current_width > max_width_SAT)
            {
                std::cout << "c Max width SAT is set to " << current_width << "\n";
                max_width_SAT = current_width;
            }
            break;
        case 20:
            if (current_width < min_width_UNSAT)
            {
                std::cout << "c Min width UNSAT is set to " << current_width << "\n";
                min_width_UNSAT = current_width;
            }
            break;
        case -10:
            std::cout << "e Get an incorrect answer.\n";
            break;
        case -20:
            std::cout << "e Get an invalid answer.\n";
            break;
        default:
            break;
        }

        abw_enc->threadCompleted = current_width;
        abw_enc->cv.notify_all();
        delete params;
        pthread_exit(nullptr);
    }

    void AntibandwidthEncoder::encode_and_solve_abw_problems(int start_w, int step, int stop_w)
    {
        int current_width = start_w;
        int number_width = stop_w - start_w;

        // Init threads
        for (int i = 0; i < thread_count && i < number_width; i += step)
        {
            auto *params = new ThreadParams{this, current_width};
            pthread_t thread_id;
            pthread_create(&thread_id, nullptr, thread_function, params);
            threads[current_width] = thread_id;

            std::cout << "c Started new thread " << current_width << "\n";
            current_width += step;

            sleep(1);
        }

        while (true)
        {
            std::unique_lock<std::mutex> lock(mtx);
            // Wait for a thread to complete
            cv.wait(lock, [this]
                    { return threadCompleted > 0; });
            sleep(1);

            // Check which thread completed and join it
            int completed_width = threadCompleted;
            auto it = threads.find(completed_width);
            if (it != threads.end())
            {
                pthread_join(it->second, nullptr);

                // Remove completed thread from map
                threads.erase(it);
                std::cout << "c Thread " << completed_width << " is completed. Removed it.\n";

                if (current_width < stop_w)
                {
                    // Start a new thread
                    auto *params = new ThreadParams{this, current_width};
                    pthread_t thread_id;
                    pthread_create(&thread_id, nullptr, thread_function, params);
                    threads[current_width] = thread_id;

                    std::cout << "c Started new thread " << current_width << "\n";
                    current_width += step;
                }
                else
                {
                    std::cout << "c Reached the end of width.\n";
                }
            }
            else
            {
                std::cout << "e Thread " << completed_width << " is not joinable or doesn't exist\n";
            }

            threadCompleted = 0;
            if (threads.empty())
                break;
        }

        // Join all threads to ensure they complete before exiting the function
        for (auto &pair : threads)
        {
            pthread_join(pair.second, nullptr);
        }
        std::cout << "c All threads have completed." << std::endl;
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
