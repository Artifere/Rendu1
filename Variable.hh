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


#ifndef CHOOSE
    #define CHOOSE BASIC
#endif
// Un peu de vaudou préprocesseur
#define FNC_CHOIX(u) chooseFromFree_ ## u
#define FNC_CHOIX_E(u) FNC_CHOIX(u)

#define chooseFromFree FNC_CHOIX_E(CHOOSE)



class Variable {
protected:
    std::vector<Clause*> _litTrue;
    std::vector<Clause*> _litFalse;
    
    std::vector<Variable*>::iterator _posInTable;
    
    // clause qui a permit de déduire la valeur de la variable
    // ou NULL si on a assigné cette valeur parce qu'on a fait le choix contraire avant et qu'on est arrivé à une contradiction
    // TODO : remplacer le cas NULL par la nouvelle clause déduite de l'erreur lorsqu'on est arrivé à une contradiction
    Clause* _deductedFromClause;

public:
    static std::vector<Variable*> _vars;
    static std::vector<Variable*>::iterator _endAssigned;
    static std::vector<Variable*>::iterator _endDeducted;

    bool _varState;
    const unsigned int varNumber;
    
    inline Variable(unsigned int varNum) : _posInTable(_vars.end()), _varState(false), varNumber(varNum) { _vars.push_back(this); };
    
    inline unsigned sizeLitTrue() const { return _litTrue.size(); };
    inline unsigned sizeLitFalse() const { return _litFalse.size(); };
    
    inline bool isFree(void) const           { return _posInTable >= _endDeducted; };
    inline bool isOlder(Variable* var) const { return _posInTable <= var->_posInTable; };
    inline bool isFromCurBet(void) const     {return false;}// return _posInTable >= _stackBacktrack.top()} // TODO

    inline Clause* getOriginClause(void) const { return _deductedFromClause; }

    void deductedFromFree(bool value, Clause* fromClause);
    Clause* assignedFromDeducted(void);
    void deductedFromAssigned(void);
    
    static void chooseFromFree_BASIC(void);
    static void chooseFromFree_DLIS(void);
    static void chooseFromFree_RAND(void);
    static void sortFreeVars(void); 
    
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



inline void Variable::deductedFromFree(bool value, Clause * fromClause)
{
    Variable * var = * (_endDeducted ++);
    std::swap(_posInTable, var->_posInTable);
    std::iter_swap(_posInTable, var->_posInTable);
    _varState = value ? true : false;
    _deductedFromClause = fromClause;
}



inline void Variable::chooseFromFree_BASIC(void)
{
    _endDeducted ++;
}



inline void Variable::chooseFromFree_DLIS(void)
{
    std::vector<Variable*>::iterator it;
    it = std::max_element(_endDeducted, _vars.end(), DLISvarCompr);
    std::swap((*_endDeducted)->_posInTable, (*it)->_posInTable);
    std::iter_swap(_endDeducted, it);
    _endDeducted ++;
}



inline void Variable::sortFreeVars(void)
{
    // on trie sans se soucier des itérateurs _posInTable
    std::sort(_endDeducted, _vars.end(), DLISvarCompr);
    // on repositionne les itérateurs _posInTable vers _vars
    std::vector<Variable*>::iterator it;
    for(it = _endDeducted; it != _vars.end(); it++)
        (*it)->_posInTable = it;
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
