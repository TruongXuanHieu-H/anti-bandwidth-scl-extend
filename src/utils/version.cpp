#include "version.h"
#include <iostream>

const std::string Version::version = "1.0.0";

const std::string &Version::get_version()
{
    return version;
}

void Version::print_version()
{
    std::cout << "c [Version] Cyclic Anti-bandwidth Encoder version " << version << ".\n";
}
