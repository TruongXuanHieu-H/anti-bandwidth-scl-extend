#include <iostream>

#include "global_data.h"
#include "abw_encoder.h"
#include "utils/signal_handler.h"
#include "utils/version.h"
#include "utils/args_parser.h"

int main(int argc, char **argv)
{
    SignalHandler::init_signals();

    Version::print_version();

    ArgsParser::init_parser();
    int parse_result = ArgsParser::try_parse_args(argc, argv);
    if (parse_result != 0)
    {
        return parse_result;
    }

    AntibandwidthEncoder abw_enc;

    if (GlobalData::just_print_dimacs)
        abw_enc.encode_and_print_dimacs();
    else
        abw_enc.encode_and_solve();

    return 0;
}
