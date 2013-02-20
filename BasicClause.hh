#include "Clause.hh"
#include <stack>
#include <set>


Class BasicClause:public Clause
{ 





  protected:
    bool _satisfied;
    stack<Literal> _falseLit;
    set<LITERAL> _free;
};
