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


// Besoin de forward declaration pour Literal et Clause  pour éviter une dépendance circulaire
   
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
    void releaseVariable(void);
    
    #if VERBOSE > 0
    void print_state(void) const;
    #endif
    
};


// Associe une clause à une variable
inline void Variable::linkToClause(bool val, Clause* c)
{
    if (val)
        _litTrue.push_back(c);
    else
        _litFalse.push_back(c);
}



/* v1 < v2 si le maximum de clauses que permet de satisfaire v1 < au même nombre pour v2
   On utilise cette comparaison pour l'heuristique statique dans le cas de choose=basic et choose = DLIS */
static inline bool DLISvarCompr(const Variable* v1, const Variable* v2)
{
    return std::max(v1->sizeLitTrue(), v1->sizeLitFalse()) < std::max(v2->sizeLitTrue(), v2->sizeLitFalse());
}



#if VERBOSE > 0
#include <iostream>
inline void Variable::print_state(void) const
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
