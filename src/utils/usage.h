#ifndef USAGE_H
#define USAGE_H

#include <map>
#include <string>
#include <iostream>
#include <iomanip>

class Helper
{
private:
    static const std::map<std::string, std::string> option_list;

public:
    static void print_usage();
};

#endif // USAGE_H