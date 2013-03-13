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





        UnassignedBucket(std::vector<Variable*>&);

    private:
        std::vector<Variable*> _unassignedList;
        std::vector<std::vector<Variable*>::iterator > _unassignedIndex;
        
        };



inline UnassignedBucket::UnassignedBucket(std::vector<Variable*> &varList)
{
//    std::cout << varList.size() << "looooooo" <<  std::endl;
    const unsigned int nbrVar = varList.size();
    _unassignedList.reserve(nbrVar);
    _unassignedIndex.resize(nbrVar);

    std::vector<Variable*>::const_iterator it;
    for (it = varList.begin(); it != varList.end(); ++it)
        addUnassigned(*it);

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
    return Literal(ret, true);
}


inline Literal UnassignedBucket::chooseRAND(void)
{
    unsigned int retId = _unassignedList.size()-1;
    Variable* ret = _unassignedList[retId];
    deleteUnassigned(ret);
    return Literal(ret, rand()%2);
}


#endif
