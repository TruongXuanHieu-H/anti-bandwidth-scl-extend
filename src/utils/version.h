#ifndef VERSION_H
#define VERSION_H

#include <string>

class Version
{
private:
    static const std::string version;

public:
    static const std::string &get_version();
    static void print_version();
};

#endif // VERSION_H
