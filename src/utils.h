#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <utility>
#include <string>

class VarHandler
{
public:
    VarHandler(int start_id, int input_size);
    int get_new_var();
    int last_var() const;
    int size() const;

private:
    int first_assigned_id;
    int next_to_assign;
    int last_intput_var;
};

#endif
