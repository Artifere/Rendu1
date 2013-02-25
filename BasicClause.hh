#ifndef BASICCLAUSE_HH
#define BASICCLAUSE_HH



#include "Clause.hh"
#include <stack>
#include <set>


class BasicClause:public Clause
{ 





  protected:
    bool _satisfied;
    stack<Literal> _falseLit;
    set<Literal> _free;
};


#endif //BASICCLAUSE defined
