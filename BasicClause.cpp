#include "BasicClause.hh"
#include <cstdlib>


BasicClause::BasicClause(const std::vector<Literal>& list)
{
  _satisfied = false;
  for (int i = 0; i < list.size(); ++i)
    _free.insert(list[i]);
}



// A bouger en non virtuelle
void BasicClause::setVar(const Literal &l)
{
  if (_free.find(l) != _free.end())
    setLitTrue(l);
  else
    setLitFalse(l);
 }


void BasicClause::setLitFalse(const Literal& l)
{
  Literal invL = l;
  invL.invert();

  _free.erase(invL);
  _assigned.push(invL);
}



void BasicClause::setLitTrue(const Literal& l)
{
  _free.erase(l);
  _assigned.push(l);

  _satisfied = true;
}



void BasicClause::freeVar(const unsigned int varId)
{
  if (!_satisfied || _assigned.top().var() == varId)
  {
    _free.insert(_assigned.top());
    _assigned.pop();
   }
}



size_t BasicClause::freeSize(void) const
{
  return _free.size();
}


size_t BasicClause::assignedSize(void) const
{
  return _assigned.size();
}


Literal BasicClause::chooseFree(void) const
{
  if (_free.empty())
    exit(1); //Modifier...
  return *_free.begin();
}

bool BasicClause::satisfied(void) const
{
  return _satisfied;
}

