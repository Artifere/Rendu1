#ifndef VARIABLE_HH
#define VARIABLE_HH

#include <vector>
#include <set>
#include <stack>


// choisir quelle implémentation de clause servira
#define UsedClause ConstAssignClause
//#define UsedClause BasicClause
//#define UsedClause BasicClauseWatched
//buggé : #define UsedClause OneWatchedClause


typedef
enum varState
{
    TRUE, FALSE, FREE
} varState;

class Literal;

class Clause;
class UsedClause;

#ifdef INLINED_CLAUSE
typedef UsedClause StockedClause;
#else
typedef Clause StockedClause;
#endif







class Variable {
protected:
    std::vector<StockedClause*> _litTrue;
    std::vector<StockedClause*> _litFalse;
public:
    const unsigned int varNumber;
    varState _varState;

    inline Variable(unsigned int varNum) : varNumber(varNum), _varState(FREE) { };
    
    
    varState getState() const;
    void setState(varState);
    
    void linkToClause(bool,StockedClause*);
    
    bool propagateVariable(std::stack<Literal>& deductions);
    void releaseVariable();
    
    #if VERBOSE > 0
    void print_state(bool complete) const;
    #endif
    
};



inline void Variable::linkToClause(bool val, StockedClause* c)
{
    if (val)
        _litTrue.push_back(c);
    else
        _litFalse.push_back(c);
}



#if VERBOSE > 0
#include <iostream>
inline void Variable::print_state(bool complete) const
{
    if (_varState == FREE)
        std::cout << "?";
    else if (_varState == FALSE)
        std::cout << "-";
    else
        std::cout << " ";
    std::cout << varNumber;
    if(complete)
        std::cout << " (" << _litTrue.size() << "x" << _litFalse.size() << ")";
}
#endif


#endif//VARIABLE_HH
