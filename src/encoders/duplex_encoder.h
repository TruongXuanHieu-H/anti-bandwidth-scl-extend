#ifndef DUPLEX_ENCODER_H
#define DUPLEX_ENCODER_H

#include "instance_encoder.h"
#include "bdd.h"

#include <deque>
#include <unordered_map>

typedef std::vector<int>::iterator vec_int_it;
typedef std::deque<int>::iterator deq_int_it;

class DuplexEncoder : public InstanceEncoder
{
public:
	DuplexEncoder();
	~DuplexEncoder();

	void encode_antibandwidth() override;

private:
	BDDHandler bh;

	std::unordered_map<int, std::vector<int>> fwd_amo_roots;
	std::unordered_map<int, std::vector<int>> bwd_amo_roots;
	std::unordered_map<int, std::vector<int>> fwd_amz_roots;
	std::unordered_map<int, std::vector<int>> bwd_amz_roots;

	std::unordered_map<int, std::vector<std::vector<int>>> node_amz_literals;
	int number_of_windows;
	int last_window_w;

	void init_members();

	void do_encode_antibandwidth();

	void construct_window_bdds(int w);
	void glue_window_bdds();

	void glue_edge_windows(int node1, int node2);
	void make_equal_bdds(BDD_id b1, BDD_id b2);
	void encode_column_eo();
	void seq_encode_column_eo();

	void product_encode_eo(const std::vector<int> &vars);
	void product_encode_amo(const std::vector<int> &vars);
	void seq_encode_amo(const std::vector<int> &vars);

	// Not & on purpose!
	BDD_id build_amo(std::deque<int> vars);
	BDD_id build_amz(std::deque<int> vars);
};

#endif
