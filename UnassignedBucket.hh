#ifndef UNASSIGNED_BUCKET_HH
#define UNASSIGNED_BUCKET_HH

#include "Variable.hh"
#include <cstdlib>
#include <vector>
#include <cstdlib>
#include <ctime>

#include <algorithm> // pour std::max_element


#ifndef CHOOSE
 #define CHOOSE BASIC
#endif
// un peu de vaudou préprocesseur
#define FNC_CHOIX(u) choose ## u
#define FNC_CHOIX_E(u) FNC_CHOIX(u)

#define chooseUnassigned FNC_CHOIX_E(CHOOSE)


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

inline void UnassignedBucket::deleteUnassigned(Variable* var)
{
    const unsigned int toDel = var->varNumber-1;
    const unsigned int last = _unassignedList.back()->varNumber-1;
    _unassignedIndex[last] = _unassignedIndex[toDel];
    *_unassignedIndex[last] = _unassignedList.back();
    _unassignedList.pop_back();
}





inline Literal UnassignedBucket::chooseBASIC(void)
{
    Variable* ret = _unassignedList.back();
    _unassignedList.pop_back();
    return Literal(ret, ret->sizeLitTrue() > ret->sizeLitFalse());
}

inline Literal UnassignedBucket::chooseRAND(void)
{
    unsigned int retId = random() % _unassignedList.size();
    Variable* ret = _unassignedList[retId];
    deleteUnassigned(ret);
    return Literal(ret, random()%2);
}

inline Literal UnassignedBucket::chooseMOMS(void)
{
    Variable* m = *std::max_element(_unassignedList.begin(), _unassignedList.end(), varCompr);
    deleteUnassigned(m);
    return Literal(m, m->sizeLitTrue() > m->sizeLitFalse());
}


#endif
