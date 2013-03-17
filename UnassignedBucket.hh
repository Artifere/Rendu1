#ifndef UNASSIGNED_BUCKET_HH
#define UNASSIGNED_BUCKET_HH
/** A FAIRE : std::swap... **/

#include "Variable.hh"
#include <vector>



class UnassignedBucket
{
    public:
        Literal chooseUnassigned(void);
        Literal chooseRAND(void);
        Variable* chooseMOMS();
        Variable* chooseDLIS();
        
        void addUnassigned(Variable *var);
        void deleteUnassigned(Variable *var);





        UnassignedBucket(const unsigned int nbrVar);

    private:
        std::vector<Variable*> _unassignedList;
        std::vector<std::vector<Variable*>::iterator > _unassignedIndex;
        
        };



inline UnassignedBucket::UnassignedBucket(const unsigned int nbrVar)//std::vector<Variable*> &varList)
{
//    std::cout << varList.size() << "looooooo" <<  std::endl;
    //const unsigned int nbrVar = varList.size();
    _unassignedList.reserve(nbrVar);
    _unassignedIndex.resize(nbrVar);

    srand(17);
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
    unsigned int retId = rand()%_unassignedList.size();
    Variable* ret = _unassignedList[retId];
    deleteUnassigned(ret);
    return Literal(ret, rand()%2);
}


#endif
