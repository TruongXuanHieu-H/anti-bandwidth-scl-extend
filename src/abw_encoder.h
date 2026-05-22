#ifndef ABW_ENCODER_H
#define ABW_ENCODER_H

#include "searchers/abw_searcher.h"

class AntibandwidthEncoder
{
public:
	AntibandwidthEncoder();
	~AntibandwidthEncoder();

	void encode_and_solve();
	void encode_and_print_dimacs();

private:
	ABWSearcher *abw_searcher = nullptr;

	void setup_searcher();
};

#endif
