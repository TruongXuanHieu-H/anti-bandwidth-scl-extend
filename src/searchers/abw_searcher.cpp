#include "abw_searcher.h"
#include "../global_data.h"
#include "../encoders/abw_instance.h"
#include "../utils/pid_manager.h"

#include <iostream>
#include <assert.h>
#include <sys/mman.h>
#include <cmath>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <chrono>

ABWSearcher::ABWSearcher()
{
    max_consumed_memory = (float *)mmap(nullptr, sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    setup_bounds();
    max_width_SAT = lower_bound - 1;
    min_width_UNSAT = upper_bound + 1;
}

ABWSearcher::~ABWSearcher()
{
    abp_pids.clear();
}

void ABWSearcher::encode_and_solve()
{
    fflush(stdout);
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time = std::chrono::high_resolution_clock::now();
    create_limit_pid();

    for (int i = 0; i < GlobalData::worker_count && i < (upper_bound + 1 - lower_bound); i += 1)
    {
        int next_width_to_seach = get_next_width_to_search();
        if (next_width_to_seach <= lower_bound - 1 || next_width_to_seach >= upper_bound + 1)
        {
            break; // No valid width to search
        }
        else
        {
            create_abp_pid(next_width_to_seach);
        }
    }

    bool limit_violated = false;

    // Parent process waits until all child processes finish
    while (!abp_pids.empty())
    {
        int status;
        pid_t finished_pid = wait(&status); // Wait for any child to complete

        if (finished_pid == lim_pid)
        {
            limit_violated = true;
            std::cout << "c [Main] Lim pid ends with result: " << WEXITSTATUS(status) << ".\n";
            while (!abp_pids.empty())
            {
                kill(abp_pids.begin()->second, SIGTERM);
                abp_pids.erase(abp_pids.begin());
            }
        }
        else if (WIFEXITED(status))
        {
            // Remove the finished child from the map
            for (auto it = abp_pids.begin(); it != abp_pids.end(); ++it)
            {
                if (it->second == finished_pid)
                {
                    // std::cout << "c Child pid " << it->first << " - " << it->second << " exited with status " << WEXITSTATUS(status) << ".\n";

                    switch (WEXITSTATUS(status))
                    {
                    case 10:
                        if (it->first > max_width_SAT)
                        {
                            max_width_SAT = it->first;
                            std::cout << "c [Main] Max width SAT is set to " << it->first << ".\n";
                        }

                        for (auto ita = abp_pids.begin(); ita != abp_pids.end(); ita++)
                        {
                            // Pid with lower width than SAT pid is also SAT.
                            if (ita->first < it->first)
                            {
                                std::cout << "c [Main] Kill lower pid " << ita->first << ".\n";
                                kill(ita->second, SIGTERM);
                            }
                        }
                        break;
                    case 20:
                        if (it->first < min_width_UNSAT)
                        {
                            min_width_UNSAT = it->first;
                            std::cout << "c [Main] Min width UNSAT is set to " << it->first << "\n";
                        }

                        for (auto ita = abp_pids.begin(); ita != abp_pids.end(); ita++)
                        {
                            // Pid with higher width than UNSAT pid is also UNSAT.
                            if (ita->first > it->first)
                            {
                                std::cout << "c [Main] Kill higher pid " << ita->first << ".\n";
                                kill(ita->second, SIGTERM);
                            }
                        }
                        break;
                    default:
                        break;
                    }

                    abp_pids.erase(it);
                    if (abp_pids.empty() && search_order.empty() && kill(lim_pid, 0) == 0)
                    {
                        kill(lim_pid, SIGTERM);
                    }
                    break;
                }
            }
        }
        else if (WIFSIGNALED(status))
        {
            // Remove the terminated child from the map
            for (auto it = abp_pids.begin(); it != abp_pids.end(); ++it)
            {
                if (it->second == finished_pid)
                {
                    std::cout << "c [Main] Child pid " << it->first << " - " << it->second << " terminated by signal " << WTERMSIG(status) << ".\n";
                    abp_pids.erase(it);
                    if (abp_pids.empty() && kill(lim_pid, 0) == 0)
                    {
                        kill(lim_pid, SIGTERM);
                    }
                    break;
                }
            }
        }
        else
        {
            for (auto it = abp_pids.begin(); it != abp_pids.end(); ++it)
            {
                if (it->second == finished_pid)
                {
                    std::cerr << "e [Main] Child pid " << it->first << " - " << it->second << " stopped or otherwise terminated.\n";
                    abp_pids.erase(it);
                    if (abp_pids.empty() && kill(lim_pid, 0) == 0)
                    {
                        kill(lim_pid, SIGTERM);
                    }
                    break;
                }
            }
        }

        if (!limit_violated)
        {
            fflush(stdout);
            while (int(abp_pids.size()) < GlobalData::worker_count)
            {
                int next_width_to_seach = get_next_width_to_search();
                if (next_width_to_seach <= lower_bound - 1 || next_width_to_seach >= upper_bound + 1)
                {
                    break; // No valid width to search
                }
                else
                {
                    create_abp_pid(next_width_to_seach);
                }
            }

            if (abp_pids.empty() && search_order.empty())
            {
                kill(lim_pid, SIGTERM);
            }
        }
    }
    std::cout << "c [Main] All children have completed.\n";

    std::chrono::time_point<std::chrono::high_resolution_clock> end_time = std::chrono::high_resolution_clock::now();
    auto encode_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    std::cout << "r [Main] \n";
    std::cout << "r [Main] Final results: \n";
    std::cout << "r [Main] Max width SAT:  \t" << ((max_width_SAT == lower_bound - 1) ? "-" : std::to_string(max_width_SAT)) << ".\n";
    std::cout << "r [Main] Min width UNSAT:\t" << ((min_width_UNSAT == upper_bound + 1) ? "-" : std::to_string(min_width_UNSAT)) << ".\n";
    std::cout << "r [Main] Total real time: " << encode_duration << " ms.\n";
    std::cout << "r [Main] Total memory consumed: " << *max_consumed_memory << " MB.\n";
    std::cout << "r [Main] \n";
}

void ABWSearcher::encode_and_print_dimacs()
{
    for (int i = lower_bound; i <= upper_bound; i++)
    {
        ABWInstance *abp_ins = new ABWInstance(i);
        abp_ins->encode_and_print_dimacs();
        delete abp_ins;
    }
};

/*
 *  Check if limit conditions are satified or not
 *  Return:
 *      0   if all the conditions is satified.
 *      -1  if out of memory.
 *      -2  if out of real time.
 *      -3  if out of elapsed time.
 */
int ABWSearcher::is_limit_satisfied()
{
    if (consumed_memory > GlobalData::memory_limit)
        return -1;

    if (consumed_real_time > GlobalData::real_time_limit)
        return -2;

    if (consumed_elapsed_time > GlobalData::elapsed_time_limit)
        return -3;

    return 0;
}

void ABWSearcher::setup_bounds()
{
    lookup_bounds();
    override_bounds();
    modify_bound();

    assert((lower_bound >= 2) && (lower_bound <= upper_bound) && (upper_bound <= GlobalData::g->n / 2));
}

void ABWSearcher::lookup_bounds()
{
    lookup_lower_bound();
    lookup_upper_bound();
}

void ABWSearcher::lookup_upper_bound()
{
    auto pos = GlobalData::abw_UBs.find(GlobalData::g->graph_name);
    if (pos != GlobalData::abw_UBs.end())
    {
        upper_bound = pos->second;
        std::cout << "c [Main] Upper bound is set to " << upper_bound << ".\n";
    }
    else
    {
        upper_bound = GlobalData::g->n / 2;
        std::cout << "c [Main] No predefined upper bound is found for " << GlobalData::g->graph_name << ".\n";
        std::cout << "c [Main] UB-w = " << upper_bound << " (default value calculated as n/2).\n";
    }
}

void ABWSearcher::lookup_lower_bound()
{
    auto pos = GlobalData::abw_LBs.find(GlobalData::g->graph_name);
    if (pos != GlobalData::abw_LBs.end())
    {
        lower_bound = pos->second;
        std::cout << "c [Main] Lower bound is set to " << lower_bound << ".\n";
    }
    else
    {
        lower_bound = 2;
        std::cout << "c [Main] No predefined lower bound is found for " << GlobalData::g->graph_name << ".\n";
        std::cout << "c [Main] LB-w = 2 (default value).\n";
    }
}

void ABWSearcher::override_bounds()
{
    override_lower_bound();
    override_upper_bound();
}

void ABWSearcher::override_lower_bound()
{
    if (GlobalData::overwrite_lb)
    {
        std::cout << "c [Main] LB " << lower_bound << " is overwritten with " << GlobalData::forced_lb << ".\n";
        lower_bound = GlobalData::forced_lb;
    }
}

void ABWSearcher::override_upper_bound()
{
    if (GlobalData::overwrite_ub)
    {
        std::cout << "c [Main] UB " << upper_bound << " is overwritten with " << GlobalData::forced_ub << ".\n";
        upper_bound = GlobalData::forced_ub;
    }
}

void ABWSearcher::modify_bound()
{
}

void ABWSearcher::create_limit_pid()
{
    lim_pid = fork();
    if (lim_pid < 0)
    {
        std::cerr << "e [Lim] Fork Failed!\n";
        exit(-1);
    }
    else if (lim_pid == 0)
    {
        pid_t main_pid = getppid();
        int limit_state = is_limit_satisfied();

        while (limit_state == 0)
        {
            consumed_memory = std::round(PIDManager::get_total_memory_usage(main_pid) * 10 / 1024.0) / 10;
            consumed_real_time += std::round((float)GlobalData::sample_rate * 10 / 1000000.0) / 10;
            consumed_elapsed_time += (float)(GlobalData::sample_rate * (PIDManager::get_descendant_pids(main_pid).size() - 1)) / 1000000.0;

            if (consumed_memory > *max_consumed_memory)
            {
                *max_consumed_memory = consumed_memory;
                // std::cout << "[Lim] Memory consumed: " << max_consumed_memory << " MB.\n";
            }

            sampler_count++;
            if (sampler_count >= GlobalData::report_rate)
            {
                // std::cout << "c [Lim] Sampler:\t" << "Memory: " << consumed_memory << " MB\tReal time: " << consumed_real_time << "s\tElapsed time: " << consumed_elapsed_time << "s.\n";
                sampler_count = 0;
            }
            usleep(GlobalData::sample_rate);

            limit_state = is_limit_satisfied();
        }

        exit(limit_state);
    }
    else
    {
        // std::cout << "c Lim pid is forked at " << lim_pid << ".\n";
    }
}

void ABWSearcher::create_abp_pid(int width)
{
    // std::cout << "p PID: " << getpid() << ", PPID: " << getppid() << ".\n";
    pid_t pid = fork();
    // std::cout << "q PID: " << getpid() << ", PPID: " << getppid() << ".\n";

    if (pid < 0)
    {
        std::cerr << "e [w = " << width << "] Fork failed!\n";
        exit(-1);
    }
    else if (pid == 0)
    {
        prctl(PR_SET_PDEATHSIG, SIGTERM);
        std::cout << "c [w = " << width << "] Start task in PID: " << getpid() << ".\n";

        // Child process: perform the task
        int result = do_abp_pid_task(width);

        exit(result);
    }
    else
    {
        // Parent process stores the child's PID
        // std::cout << "c Child pid " << width << " - " << pid << " is tracked in PID: " << getpid() << ".\n";
        abp_pids[width] = pid;
    }
}

int ABWSearcher::do_abp_pid_task(int width)
{
    // Dynamically allocate and use ABPEncoder in child process
    ABWInstance *abp_ins = new ABWInstance(width);

    int result = abp_ins->encode_and_solve_abp();

    std::cout << "c [w = " << width << "] Result: " << result << ".\n";

    // Clean up dynamically allocated memory
    delete abp_ins;

    // std::cout << "c [w = " << width << "] Child " << width << " completed task.\n";
    return result;
}

int ABWSearcher::get_next_width_to_search()
{
    if (search_order.empty())
        return lower_bound - 1; // To terminate the search if no valid width is left. Value (upper_bound + 1) also works.

    int next_width = search_order.front();
    search_order.pop_front();

    while (next_width <= max_width_SAT || next_width >= min_width_UNSAT)
    {
        if (search_order.empty())
            return lower_bound - 1; // To terminate the search if no valid width is left. Value (upper_bound + 1) also works.

        next_width = search_order.front();
        search_order.pop_front();
    }

    return next_width;
}
