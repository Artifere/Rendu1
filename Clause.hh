#ifndef CLAUSE_HH
#define CLAUSE_HH



#include "Literal.hh"

#include <vector>

#include <cstddef>
using namespace std;

class Clause
{
  public:
    virtual void setVar(const Literal &l)=0;
    virtual void setLitFalse(const Literal& l)=0;
    virtual void setLitTrue(const Literal& l)=0;
    virtual void freeVar(const unsigned int varId)=0;
    virtual size_t freeSize (void) const=0;
    virtual size_t assignedSize(void) const=0;
    virtual Literal chooseFree(void) const=0;
    virtual bool satisfied(void) const = 0;
};


#endif //CLAUSE_HH defined
