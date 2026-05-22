#ifndef VAR_HANDLER_H
#define VAR_HANDLER_H
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