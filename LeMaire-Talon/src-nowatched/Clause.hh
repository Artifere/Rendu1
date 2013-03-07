#ifndef CLAUSE_HH
#define CLAUSE_HH

#include "Literal.hh"
#include <vector>


class Clause
{
public:
    
    virtual void setLitFalse(const Literal& l)=0;
    virtual void setLitTrue(const Literal& l)=0;

    virtual void freeLitFalse(const Literal &l)=0;
    virtual void freeLitTrue(const Literal &l)=0;

    virtual size_t freeSize (void) const=0;
    virtual Literal chooseFree(void) const=0;
    virtual bool satisfied(void) const=0;

    virtual ~Clause() {}
};


#endif //CLAUSE_HH
