#ifndef CONSTASSIGNCLAUSE_HH
#define CONSTASSIGNCLAUSE_HH

// Pour intptr_t
#include <stdint.h>


/***
   * Cette version est l'ancêtre de SmartClause : elle est plus lente que cette dernière et ne
   * présente donc plus d'intérêt. Elle a néanmoins été élaborée entre les rendus 1 et 2
***/

class ConstAssignClause
{
public:
    ConstAssignClause(const CONSTR_ARGS(list));

    bool setLitFalse(const Literal& l);
    bool setLitTrue(const Literal& l);

    void freeLitFalse(const Literal &l);
    void freeLitTrue(const Literal &l);

    unsigned int freeSize (void) const;
    Literal getRemaining(void) const;
    bool isSatisfied(void) const;

    ~ConstAssignClause();

    #if VERBOSE > 1
    const unsigned _number;
    #endif

protected:
    // Somme des adresses des variables libres contenues dans la clause
    intptr_t _currentHash;
    // XOR des polarités des litéraux libres contenus dans la clause
    bool _currentHashVal;
    // Adresse de la première variable mise à vrai dans la clause
    Variable* _satisfied;
    // Nombere de variables libres restantes (taille de la partie « vivante » de la clause)
    unsigned int _numOfFree;
};





/***
 * Implémentation des méthodes de la classe
 * (toutes inlines)
***/

/* Le constructeur initialise les différents « hash » et lie la clause à toutes
   les variables qu'elle contient */
inline ConstAssignClause::ConstAssignClause(const CONSTR_ARGS(list))
    : INIT_FOR_VERBOSE()  _currentHash((intptr_t)NULL), _currentHashVal(false), _satisfied(NULL), _numOfFree(list.size())
{
    std::vector<Literal>::const_iterator it;
    for(it = list.begin(); it != list.end(); ++it)
    {
        _currentHash += (intptr_t)it->var();
        _currentHashVal = (_currentHashVal != it->pos()); // XOR booléen
        it->var()->linkToClause(it->pos(), (Clause*)this);
    }
}


// Met un litéral à faux dans la clause. On met donc à jour les hash et le nombre de variables libres.
inline bool ConstAssignClause::setLitFalse(const Literal& l)
{
    if(_satisfied == NULL)
    {
        _currentHash -= (intptr_t)l.var();
        _currentHashVal = (_currentHashVal != !l.pos()); // XOR booléen
        _numOfFree--;
    }
    return false;
}


// Idem que pour setLitFalse, mais on met en plus à jour l'adresse de la première variable vraie
inline bool ConstAssignClause::setLitTrue(const Literal& l)
{
    if(_satisfied == NULL)
        _satisfied = l.var();
    return false;
}


// On n'a rien à faire hormis signaler que la clause n'est plus vraie
inline void ConstAssignClause::freeLitTrue(const Literal& l)
{
    if(_satisfied == l.var())
        _satisfied = NULL;
}


// Il faut mettre à jour les hash et le nombre de variables libres
inline void ConstAssignClause::freeLitFalse(const Literal& l)
{
    if(_satisfied == NULL)
    {
        _currentHash += (intptr_t)l.var();
        _currentHashVal = (_currentHashVal != !l.pos()); // XOR booléen
        _numOfFree++;
    }
}



inline unsigned int ConstAssignClause::freeSize (void) const
{
    return _numOfFree;
}


// Cette fonction est appelée quand il ne reste plus qu'une variable libre, et renvoie le litéral en question
inline Literal ConstAssignClause::getRemaining(void) const
{
    return Literal((Variable*)_currentHash, _currentHashVal);
}



inline bool ConstAssignClause::isSatisfied(void) const
{
    return _satisfied;
}



inline ConstAssignClause::~ConstAssignClause()
{
}


#endif //CONSTASSIGNCLAUSE_HH defined
