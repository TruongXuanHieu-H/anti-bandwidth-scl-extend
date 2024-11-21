#ifndef ABW_ENCODER_H
#define ABW_ENCODER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include <climits>

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
		int thread_count = 1;
		int threadCompleted = 0;
		static int max_width_SAT;
		static int min_width_UNSAT;

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

		static void *thread_function(void *args);

		struct ThreadParams
		{
			AntibandwidthEncoder *abw_enc;
			int current_width;
		};

	private:
		static std::unordered_map<int, pthread_t> threads;
		std::mutex mtx;
		std::condition_variable cv;

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
