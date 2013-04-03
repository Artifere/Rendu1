#ifndef VARIABLE_HH
#define VARIABLE_HH

#include <vector>
#include <stack>
#include <algorithm>


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
    
    std::vector<Variable*>::iterator _posInTable;

public:
    static std::vector<Variable*> _vars;
    static std::vector<Variable*>::iterator _endAssigned;
    static std::vector<Variable*>::iterator _endDeducted;

    bool _varState;
    const unsigned int varNumber;

    inline Variable(unsigned int varNum) : _posInTable(_vars.end()), _varState(false), varNumber(varNum) { _vars.push_back(this); };
    
    inline unsigned sizeLitTrue() const { return _litTrue.size(); };
    inline unsigned sizeLitFalse() const { return _litFalse.size(); };
    
    inline bool isFree(void) const { return _posInTable >= _endDeducted; }

    void deductedFromFree(bool value);
    bool assignedFromDeducted(void);
    void deductedFromAssigned(void);
    
    static void choseFromFree_BASIC(void);
    static void choseFromFree_DLIS(void);
    static void choseFromFree_RAND(void);
    
    void linkToClause(bool,Clause*);
    
    #if VERBOSE > 0
    void print_state(void) const;
    #endif
    
};
/* v1 < v2 si le maximum de clauses que permet de satisfaire v1 < au même nombre pour v2
   On utilise cette comparaison pour l'heuristique statique dans le cas de choose=basic et choose = DLIS */
static inline bool DLISvarCompr(const Variable* v1, const Variable* v2)
{
    return std::max(v1->sizeLitTrue(), v1->sizeLitFalse()) < std::max(v2->sizeLitTrue(), v2->sizeLitFalse());
}



inline void Variable::deductedFromFree(bool value)
{
    Variable * var = * (_endDeducted ++);
    std::swap(_posInTable, var->_posInTable);
    std::iter_swap(_posInTable, var->_posInTable);
    _varState = value ? true : false;
}



inline void Variable::choseFromFree_BASIC()
{
    _endDeducted ++;
}



inline void Variable::choseFromFree_DLIS()
{
    std::vector<Variable*>::iterator it;
    it = std::max_element(_endDeducted, _vars.end(), DLISvarCompr);
    std::swap((*_endDeducted)->_posInTable, (*it)->_posInTable);
    std::iter_swap(_endDeducted, it);
    _endDeducted ++;
}



// Associe une clause à une variable
inline void Variable::linkToClause(bool val, Clause* c)
{
    if (val)
        _litTrue.push_back(c);
    else
        _litFalse.push_back(c);
}



#if VERBOSE > 0
#include <iostream>
inline void Variable::print_state(void) const
{
    if (isFree())
        std::cout << "?";
    else if (_varState)
        std::cout << " ";
    else
        std::cout << "-";
    std::cout << varNumber;
}
#endif


#endif//VARIABLE_HH
