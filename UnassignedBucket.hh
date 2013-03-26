#ifndef UNASSIGNED_BUCKET_HH
#define UNASSIGNED_BUCKET_HH

#include <cstdlib>
#include <vector>
#include <ctime>
#include <algorithm>

#include "Variable.hh"

#ifndef CHOOSE
    #define CHOOSE BASIC
#endif
// Un peu de vaudou préprocesseur
#define FNC_CHOIX(u) choose ## u
#define FNC_CHOIX_E(u) FNC_CHOIX(u)

#define chooseUnassigned FNC_CHOIX_E(CHOOSE)


// Cette classe sert pour le choix de variables libres pour les paris
class UnassignedBucket
{
public:
    UnassignedBucket(const unsigned int nbrVar);

    void addUnassigned(Variable *var);
    void deleteUnassigned(Variable *var);
    
    Literal chooseBASIC(void);
    Literal chooseRAND(void);
    Literal chooseMOMS(void);
    Literal chooseDLIS(void);

private:
    
    std::vector<Variable*> _unassignedList;
    std::vector<std::vector<Variable*>::iterator> _unassignedIndex;
    
};




/* L'utilisation d'un tableaux d'itérateurs vers les éléments de _unassignedList requiert
   que ce vecteur ne soit pas déplacé en mémoire. C'est pourquoi on réserve sa taille. */
inline UnassignedBucket::UnassignedBucket(const unsigned int nbrVar)
    : _unassignedList(), _unassignedIndex(nbrVar)
{
    _unassignedList.reserve(nbrVar);
    srandom(time(NULL));
}
          




inline void UnassignedBucket::addUnassigned(Variable* var)
{
    _unassignedIndex[var->varNumber-1] = _unassignedList.end();
    _unassignedList.push_back(var);
}


/* Pour supprimer un élément, on l'échange avec le dernier. Il faut mettre à jour les itérateurs
   de _unassignedIndex. */
inline void UnassignedBucket::deleteUnassigned(Variable* var)
{
    const unsigned int toDel = var->varNumber-1;
    const unsigned int last = _unassignedList.back()->varNumber-1;
    
    _unassignedIndex[last] = _unassignedIndex[toDel];
    *_unassignedIndex[last] = _unassignedList.back();
    _unassignedList.pop_back();
}



// On choisit le dernier litéral de la liste, avec une petite heuristique sur sa polarité
inline Literal UnassignedBucket::chooseBASIC(void)
{
    Variable* ret = _unassignedList.back();
    _unassignedList.pop_back();
    return Literal(ret, ret->sizeLitTrue() > ret->sizeLitFalse());
}



/* Choisit une position aléatoire et renvoie l'élément de _unassignedList à cette position.
   Random est normalement plus rapide et plus « aléatoire » que rand. */
inline Literal UnassignedBucket::chooseRAND(void)
{
    unsigned int retId = random() % _unassignedList.size();
    Variable* ret = _unassignedList[retId];
    deleteUnassigned(ret);
    return Literal(ret, random()%2);
}



/* Heuristique MOMS. On parcourt toutes les variables pour déterminer la meilleure. Ce choix se base sur les
   informations que l'on a sur chaque variable. */
inline Literal UnassignedBucket::chooseMOMS(void)
{
    Variable* m = *std::max_element(_unassignedList.begin(), _unassignedList.end(), varCompr);
    deleteUnassigned(m);
    return Literal(m, m->sizeLitTrue() > m->sizeLitFalse());
}


#endif //UNASSIGNED_BUCKET_HH defined
