#ifndef CLAUSE_HH
#define CLAUSE_HH

#include "Literal.hh"


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




#ifndef INLINED_CLAUSE
    #define VIRTUAL virtual
    #define HERITED_CLAUSE  : public Clause
#else
    #define VIRTUAL 
    #define HERITED_CLAUSE 
#endif




#include "BasicClause.hh"
#include "ConstAssignClause.hh"


#endif //CLAUSE_HH
