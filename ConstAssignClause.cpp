#include "ConstAssignClause.hh" 
 
ConstAssignClause::ConstAssignClause(const std::vector<Literal>& list)
    : Clause(), _initLit(), _initHash(0), _currentHash(0), _satisfied(0), _numOfFree(list.size())
{
    for(unsigned int i = 0; i < list.size(); i++)
    {
        _initHash += hashOfLit(list[i]);
        _initLit.insert(list[i]);
    }
}
    

void ConstAssignClause::setVar(const Literal &l)
{
    if(_initLit.find(l) == _initLit.end())
        setLitFalse(l);
    else
        setLitTrue(l);
}

void ConstAssignClause::setLitFalse(const Literal& l)
{
    if(!_satisfied)
    {
        _currentHash += hashOfInvertLit(l);
        _numOfFree--;
    }
}

void ConstAssignClause::setLitTrue(const Literal& l)
{
    if(!_satisfied)
        _satisfied = l.var();
}

void ConstAssignClause::freeVar(const unsigned int varID)
{
    if(!_satisfied)
    {
        _currentHash -= hashOfLit(Literal(varID, _initLit.find(Literal(varID,true)) != _initLit.end()));
        _numOfFree++;
    }
    else if(_satisfied == varID)
    {
        _satisfied = 0;
    }
}
void ConstAssignClause::freeLitTrue(const Literal& l)
{
    if(_satisfied == l.var())
        _satisfied = 0;
}
void ConstAssignClause::freeLitFalse(const Literal& l)
{
    if(!_satisfied)
    {
        _currentHash -= hashOfInvertLit(l);
        _numOfFree++;
    }
}


size_t ConstAssignClause::freeSize (void) const
{
    return _satisfied ? 0 : _numOfFree;
}

size_t ConstAssignClause::assignedSize(void) const
{
    return 0;
}

Literal ConstAssignClause::chooseFree(void) const
{
    return LitOfHash(_initHash - _currentHash);
}
bool ConstAssignClause::satisfied(void) const
{
    return _satisfied;
}

ConstAssignClause::~ConstAssignClause() { }

