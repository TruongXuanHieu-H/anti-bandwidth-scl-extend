#ifndef ABW_SEARCHER_ITERATE_FROM_LB_H
#define ABW_SEARCHER_ITERATE_FROM_LB_H

#include "abw_searcher.h"

class ABWSearcherIterateFromLB : public ABWSearcher
{
public:
    ABWSearcherIterateFromLB();
    ~ABWSearcherIterateFromLB() = default;

protected:
    std::deque<int> create_search_order() override;
};

#endif // ABW_SEARCHER_ITERATE_FROM_LB_H