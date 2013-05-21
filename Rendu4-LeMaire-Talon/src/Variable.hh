#ifndef VARIABLE_HH
#define VARIABLE_HH

#include <cstdlib>
#include <ctime>
#include <vector>
#include <stack>
#include <algorithm>
#include <iostream>



// On a besoin de forward declaration pour Literal et Clause pour éviter une dépendance circulaire

class Literal;

#ifndef CLAUSE
    #define CLAUSE Clause
#endif
class CLAUSE;
typedef CLAUSE Clause;


#ifndef CHOOSE
    #define CHOOSE BASIC
#endif
// Un peu de vaudou préprocesseur...
#define FNC_CHOIX(u) chooseFromFree_ ## u
#define FNC_CHOIX_E(u) FNC_CHOIX(u)

#define chooseFromFree FNC_CHOIX_E(CHOOSE)



class Variable {
protected:
    std::vector<Clause*> _litTrue;
    std::vector<Clause*> _litFalse;
    
    std::vector<Variable*>::iterator _posInTable;
    
    // Contiennent les clauses qui ont permis de déduire la valeur (vrai ou faux) de la variable
    
    Clause* _deductedTrueFromClause;
    Clause* _deductedFalseFromClause;

public:
    static std::vector<Variable*> _vars;
    static std::vector<Variable*>::iterator _endAssigned;
    static std::vector<Variable*>::iterator _endDeducted;

    bool _varState;
    const unsigned int varNumber;
    
    inline Variable(unsigned int varNum) : _posInTable(_vars.end()), _deductedTrueFromClause(NULL), _deductedFalseFromClause(NULL), _varState(false), varNumber(varNum) { _vars.push_back(this); };
    
    inline unsigned sizeLitTrue() const { return _litTrue.size(); };
    inline unsigned sizeLitFalse() const { return _litFalse.size(); };
    
    inline bool isOlder(Variable* var) const { return _posInTable <= var->_posInTable; };
    inline bool isOlderIter(const std::vector<Variable*>::iterator& iter) const     {return _posInTable >= iter;}
    inline bool isFree(void) const           { return isOlderIter(_endAssigned); };

    inline Clause* getOriginClause(bool value) const
    {
        return (value ? _deductedTrueFromClause:_deductedFalseFromClause);
    }

    void deductedFromFree(bool value, Clause* fromClause);
    Variable* assignedFromDeducted(void);
    void deductedFromAssigned(void);
    
    static void chooseFromFree_BASIC(void);
    static void chooseFromFree_DLIS(void);
    static void chooseFromFree_MOMS(void);
    static void chooseFromFree_RAND(void);
    static void sortFreeVars(void); 
    
    void linkToClause(bool,Clause*);
    
    void moveToFirstAssign(void);

    friend std::ostream& operator<< (std::ostream& out, const Variable&);
};

/* v1 < v2 si le maximum de clauses que permet de satisfaire v1 < au même nombre pour v2
   On utilise cette comparaison pour l'heuristique statique dans le cas de choose=basic et choose=DLIS */
static inline bool DLISvarCompr(const Variable* v1, const Variable* v2)
{
    return std::max(v1->sizeLitTrue(), v1->sizeLitFalse()) < std::max(v2->sizeLitTrue(), v2->sizeLitFalse());
}



/* Remplit la variable de la classe Variable concernant la clause de déduction de cette variable,
   et met à jour les potitions de fin de variables dont la valeur est fixée et des variables inconnues ou à déduire */
inline void Variable::deductedFromFree(bool value, Clause * fromClause)
{
    Variable * var = * (_endDeducted ++);
    std::swap(_posInTable, var->_posInTable);
    std::iter_swap(_posInTable, var->_posInTable);
    
    if (value)
        _deductedTrueFromClause = fromClause;
    else
        _deductedFalseFromClause = fromClause;
    _varState = value;
}





/* Fonction de tri pour l'option SORT de tri des variables juste avant le début de résolution */
inline void Variable::sortFreeVars(void)
{
    // on trie sans se soucier des itérateurs _posInTable
    std::sort(_endDeducted, _vars.end(), DLISvarCompr);
    // on repositionne les itérateurs _posInTable vers _vars
    std::vector<Variable*>::iterator it;
    for(it = _endDeducted; it != _vars.end(); ++it)
        (*it)->_posInTable = it;
}



/* Associe une clause à une variable, quand on crée la clause, ou bien quand on avait déliée la variable
   de la clause parce que la variable était assignée */
inline void Variable::linkToClause(bool val, Clause* c)
{
    if (val)
        _litTrue.push_back(c);
    else
        _litFalse.push_back(c);
}



#endif//VARIABLE_HH
