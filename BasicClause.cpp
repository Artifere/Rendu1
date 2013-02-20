#include "BasicClause.hh"
#include <cstdlib>



// A bouger en non virtuelle
void BasicClause::setVar(const Literal &l)
{
  if (_free.find(Literal &l) != _free.end())
    setLitTrue(l);
  else
    setLifFalse(l);
 }


void BasicClause::setLitFalse(const Literal& l)
{
  Literal invL = l;
  invL.invert();

  _free.remove(invL);
  _assigned.push(invL);
}



void BasicClause::setLitTrue(const Literal& l)
{
  _free.remove(l);
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



size_t BasicClause::freeSize(void)
{
  return _free.size();
}


size_t BasicClause::assignedSize(void)
{
  return _assigned.size();
}


Literal BasicClause::chooseFree(void)
{
  if (_free.empty())
    exit(1); //Modifier...
  return *_free.begin();
}

bool BasicClause::satisfied(void)
{
  return _satisfied;
}

