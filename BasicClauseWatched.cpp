#include "BasicClauseWatched.hh"
#include <cstdlib>


BasicClauseWatched::BasicClauseWatched(const std::vector<Literal>& list)
{
    _satisfied = false;
    for (size_t i = 0; i < list.size(); ++i)
        _free.insert(list[i]);
}



// A bouger en non virtuelle
void BasicClauseWatched::setVar(const Literal &l)
{
    if (_free.find(l) != _free.end())
        setLitTrue(l);
    else
        setLitFalse(l);
}


void BasicClauseWatched::setLitFalse(const Literal& l)
{
    if(! _satisfied)
    {
        Literal invL = l;
        invL.invert();

        _free.erase(invL);
        _assigned.push(invL);
    }
}



void BasicClauseWatched::setLitTrue(const Literal& l)
{
    if(! _satisfied)
    {
        _free.erase(l);
        _assigned.push(l);

        _satisfied = true;
    }
}



void BasicClauseWatched::freeVar(const unsigned int varId)
{
    if (!_assigned.empty() && _assigned.top().var() == varId)
    {
        _free.insert(_assigned.top());
        _assigned.pop();
        _satisfied = false;
    }
}
void BasicClauseWatched::freeLitTrue(const Literal &l)
{
    freeVar(l.var());
}
void BasicClauseWatched::freeLitFalse(const Literal &l)
{
    freeVar(l.var());
}


size_t BasicClauseWatched::freeSize(void) const
{
    return _free.size();
}


size_t BasicClauseWatched::assignedSize(void) const
{
    return _assigned.size();
}


Literal BasicClauseWatched::chooseFree(void) const
{
    if (_free.empty())
        exit(1); //Modifier...
    return *_free.begin();
}

bool BasicClauseWatched::satisfied(void) const
{
    return _satisfied;
}



BasicClauseWatched::~BasicClauseWatched()
{
}
