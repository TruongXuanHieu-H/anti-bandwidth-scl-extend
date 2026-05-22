#ifndef ARGS_PARSER_H
#define ARGS_PARSER_H

#include <unordered_map>
#include <functional>
#include <string>
#include <stdexcept>

using Command = std::function<void(int &, int, char **)>;

class ArgsParser
{
public:
    static void init_parser();
    static int try_parse_args(int argc, char **argv);

private:
    static std::unordered_map<std::string, Command> cmd;
    static int get_int(const std::string &arg);
    static int get_positive(int &i, int argc, char **argv, const std::string &name);
};

#endif // ARGS_PARSER_H