#ifndef ABW_ENCODER_H
#define ABW_ENCODER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <climits>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "encoder.h"

namespace SATABP
{

	enum EncoderStrategy
	{
		duplex,
		reduced,
		seq,
		product,
		ladder,
	};

	enum IterativeStrategy
	{
		from_lb,
		from_ub,
		bin_search,
	};

	class AntibandwidthEncoder
	{
	public:
		AntibandwidthEncoder();
		virtual ~AntibandwidthEncoder();

		Graph *graph;
		int process_count = 1;

		int max_width_SAT = INT_MIN;
		int min_width_UNSAT = INT_MAX;

		EncoderStrategy enc_choice = duplex;
		IterativeStrategy iterative_strategy = from_lb;

		// Solver configurations
		bool force_phase = false;
		bool verbose = true;
		std::string sat_configuration = "sat";

		bool enable_solution_verification = true;
		int split_limit = 0;
		std::string symmetry_break_strategy = "n";

		bool overwrite_lb = false;
		bool overwrite_ub = false;
		int forced_lb = 0;
		int forced_ub = 0;

		void read_graph(std::string graph_file_name);
		void encode_and_solve_abws();
		void encode_and_print_abw_problem(int w);

		void create_child_pid(int width);
		int do_child_pid_task(int width);

	private:
		std::unordered_map<int, pid_t> child_pids;

		void encode_and_solve_abw_problems_from_lb();
		void encode_and_solve_abw_problems_from_ub();
		void encode_and_solve_abw_problems_bin_search();

		void encode_and_solve_abw_problems(int w_from, int w_to, int stop_w);
		bool encode_and_solve_antibandwidth_problem(int w);

		void lookup_bounds(int &lb, int &ub);
		void setup_bounds(int &w_from, int &w_to);

		static std::unordered_map<std::string, int> abw_LBs;
		static std::unordered_map<std::string, int> abw_UBs;
	};

}

#endif
