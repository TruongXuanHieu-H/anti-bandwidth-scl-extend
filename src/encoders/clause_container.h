#ifndef CLAUSE_CONTAINER_H
#define CLAUSE_CONTAINER_H

#include <vector>
#include "var_handler.h"

typedef std::vector<int> Clause;
typedef std::vector<Clause> Clauses;

class ClauseContainer
{
public:
	ClauseContainer();
	~ClauseContainer();

	Clauses clause_list;

	void add_clause(const Clause &c);
	int size();

protected:
	bool do_split = false;

private:
	void do_add_clause(const Clause &c);
	int do_size();
	void do_clear();
};

#endif
