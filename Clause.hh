#ifndef CLAUSE_HH
#define CLAUSE_HH



#include "Literal.hh"
#include <vector>
#include <cstddef>

class Clause
{
public:
    
    virtual void setVar(const Literal &l);
    virtual void setLitFalse(const Literal& l);
    virtual void setLitTrue(const Literal& l);

    virtual void freeVar(const unsigned int varId);
    virtual void freeLitFalse(const Literal &l);
    virtual void freeLitTrue(const Literal &l);

    virtual size_t freeSize (void) const=0;
    virtual size_t assignedSize(void) const=0;
    virtual Literal chooseFree(void) const=0;
    virtual bool satisfied(void) const=0;


    virtual ~Clause() {}
};


#endif //CLAUSE_HH
