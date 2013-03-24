#ifndef UNASSIGNED_BUCKET_HH
#define UNASSIGNED_BUCKET_HH

#include "Variable.hh"
#include <cstdlib>
#include <vector>
#include <cstdlib>


class UnassignedBucket
{
public:
    UnassignedBucket(const unsigned int nbrVar);

    Literal chooseUnassigned(void);
    Literal chooseRAND(void);
    Literal chooseMOMS();
    Literal chooseDLIS();
    
    void addUnassigned(Variable *var);
    void deleteUnassigned(Variable *var);

private:
       
    std::vector<Variable*> _unassignedList;
    std::vector<std::vector<Variable*>::iterator > _unassignedIndex;
    
};





inline UnassignedBucket::UnassignedBucket(const unsigned int nbrVar)
    : _unassignedList(), _unassignedIndex(nbrVar)
{
    _unassignedList.reserve(nbrVar);
    srandom(17);
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





inline Literal UnassignedBucket::chooseUnassigned(void)
{
    Variable* ret = _unassignedList.back();
    _unassignedList.pop_back();
    return Literal(ret, ret->sizeLitTrue() > ret->sizeLitFalse());
}

inline Literal UnassignedBucket::chooseRAND(void)
{
    unsigned int retId = random()%_unassignedList.size();
    Variable* ret = _unassignedList[retId];
    deleteUnassigned(ret);
    return Literal(ret, random()%2);
}


#endif
