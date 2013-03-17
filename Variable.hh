#ifndef VARIABLE_HH
#define VARIABLE_HH

#include <vector>
#include <set>
#include <stack>


// choisir quelle implémentation de clause servira
//#define UsedClause ConstAssignClause
//#define UsedClause BasicClauseWatched
//#define UsedClause BasicClause
#define UsedClause SmartClause
//#define UsedClause SmartClauseWatched
//buggé : #define UsedClause OneWatchedClause


typedef
enum varState
{
    TRUE, FALSE, FREE
} varState;

class Literal;


#ifdef INLINED_CLAUSE
class UsedClause;
typedef UsedClause StockedClause;
#else
class Clause;
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
    
    inline unsigned sizeLitTrue() const { return _litTrue.size(); };
    inline unsigned sizeLitFalse() const { return _litFalse.size(); };
    
    void linkToClause(bool,StockedClause*);
    
    bool propagateVariable(std::stack<Literal>& deductions);
    void releaseVariable();
    
    #if VERBOSE > 0
    void print_state() const;
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
inline void Variable::print_state() const
{
    if (_varState == FREE)
        std::cout << "?";
    else if (_varState == FALSE)
        std::cout << "-";
    else
        std::cout << " ";
    std::cout << varNumber;
}
#endif


#endif//VARIABLE_HH
