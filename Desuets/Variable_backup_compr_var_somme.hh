#ifndef VARIABLE_HH
#define VARIABLE_HH

#include <vector>
#include <stack>


// L'état d'assignation d'une variable
typedef
enum varState
{
    TRUE, FALSE, FREE
} varState;


// Besoin de forward declaration de Literal pour éviter une dépendance circulaire
class Literal;

#ifndef CLAUSE
    #define CLAUSE BasicClause
#endif
class CLAUSE;
typedef CLAUSE Clause;




class Variable {
protected:
    std::vector<Clause*> _litTrue;
    std::vector<Clause*> _litFalse;
public:
    const unsigned int varNumber;
    varState _varState;

    inline Variable(unsigned int varNum) : varNumber(varNum), _varState(FREE) { };
    
    inline unsigned sizeLitTrue() const { return _litTrue.size(); };
    inline unsigned sizeLitFalse() const { return _litFalse.size(); };
    
    
    void linkToClause(bool,Clause*);
    
    bool propagateVariable(std::stack<Literal>& deductions);
    void releaseVariable();
    
    #if VERBOSE > 0
    void print_state() const;
    #endif
    
};



inline void Variable::linkToClause(bool val, Clause* c)
{
    if (val)
        _litTrue.push_back(c);
    else
        _litFalse.push_back(c);
}




static inline bool varCompr(const Variable* v1, const Variable* v2)
{
    return std::max(v1->sizeLitTrue(), v1->sizeLitFalse()) < std::max(v2->sizeLitTrue(), v2->sizeLitFalse());
    //return v1->sizeLitTrue() + v1->sizeLitFalse() < v2->sizeLitTrue() + v2->sizeLitFalse();
    //eturn v2->sizeLitTrue() + v2->sizeLitFalse() < v1->sizeLitTrue() + v1->sizeLitFalse(); // 5.5s pour WatchedClause sur unsat-41
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
