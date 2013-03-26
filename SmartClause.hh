#ifndef SMARTCLAUSE_HH
#define SMARTCLAUSE_HH

#include <stdint.h> // pour intptr_t
#include <vector>



class SmartClause
{
public:
    SmartClause(const CONSTR_ARGS(list));

    bool setLitFalse(const Literal &l);
    bool setLitTrue(const Literal &l);

    void freeLitFalse(const Literal &l);
    void freeLitTrue(const Literal &l);

    unsigned int freeSize (void) const;
    Literal getRemaining(void) const;
    bool isSatisfied(void) const;

    ~SmartClause();

    #if VERBOSE > 1
    const unsigned _number;
    #endif
protected:
    // Somme des adresses des variables libres contenues dans la clause
    intptr_t _currentHash;
    // XOR des polarités des litéraux libres contenus dans la clause
    bool _currentHashVal;
    bool _satisfied;
    // Nombere de variables libres restantes (taille de la partie « vivante » de la clause)
    unsigned int _numOfFree;
    // Liste des litéraux que l'on a arrêté de « surveiller » après avoir satisfait une clause
    std::vector<Literal> _notWatched;
    };





/***
 * Implémentation des méthodes de la classe
 * (toutes inlines)
***/

/* le constructeur initialise les différents « hash » et lie la clause à toutes
   les variables qu'elle contient */
inline SmartClause::SmartClause(const CONSTR_ARGS(list))
    : INIT_FOR_VERBOSE()  _currentHash((intptr_t)NULL), _currentHashVal(false), _satisfied(false), _numOfFree(list.size()), _notWatched(0)
{
    std::vector<Literal>::const_iterator it;
    for(it = list.begin(); it != list.end(); ++it)
    {
        _currentHash += (intptr_t)it->var();
        _currentHashVal = (_currentHashVal != it->pos()); // XOR booléen
        it->var()->linkToClause(it->pos(), (Clause*)this);
    }
}



/* Met un litéral à faux dans la clause. Si la clause est satisfaite, on arrête de la « surveiller » via ce litéral.
   Sinon, on met à jour les hash et le nombre de variables libres. */

inline bool SmartClause::setLitFalse(const Literal &l)
{
    if(_satisfied)
        _notWatched.push_back(l.invert());
    else
    {
        _currentHash -= (intptr_t)l.var();
        _currentHashVal = (_currentHashVal != !l.pos()); // XOR booléen avec l.invert().pos()
        _numOfFree--;
    }
    return _satisfied;
}



// Idem que pour setLitFalse, mais on n'a pas besoin de mettre à jour les hash et le nombre de variables libres.
inline bool SmartClause::setLitTrue(const Literal &l)
{
    const bool res = _satisfied;
    _satisfied = true;
    if(res)
        _notWatched.push_back(l);
    return res;
}


/* Comme on arête de surveiller un litéral dès que la clause devient satisfaite, si on appelle
   freeLitTrue, c'est forcément sur le premier litéral qui l'a rendue satisfaite. La clause
   n'est ainsi plus satisfiable, et on resurveille donc tous ses litéraux. */
inline void SmartClause::freeLitTrue(const Literal& l)
{
    _satisfied = false;
    std::vector<Literal>::const_iterator it;
    for(it = _notWatched.begin(); it != _notWatched.end(); it++)
        it->var()->linkToClause(it->pos(), (Clause*)this);
    _notWatched.clear();
}



/* Comme on arête de surveiller un litéral une fois que la clause devient satisfaite, on n'a pas
   besoin de vérifier qu'on a bien mis à jour le hash et le nombre de variables libres pour ce
   litéral : il a bien été mis à faux si l'on appelle freeLitFalse. */
inline void SmartClause::freeLitFalse(const Literal& l)
{
    _currentHash += (intptr_t)l.var();
    _currentHashVal = (_currentHashVal != !l.pos()); // XOR booléen avec l.invert().pos()
    _numOfFree++;
}



inline unsigned int SmartClause::freeSize (void) const
{
    return _numOfFree;
}



// Cette fonction est appelée quand il ne reste plus qu'une variable libre, et renvoie le litéral en question
inline Literal SmartClause::getRemaining(void) const
{
    return Literal((Variable*)_currentHash, _currentHashVal);
}



inline bool SmartClause::isSatisfied(void) const
{
    return _satisfied;
}



inline SmartClause::~SmartClause()
{
}

#endif // SMARTCLAUSE_HH defined
