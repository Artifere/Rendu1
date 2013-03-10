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


#if VERBOSE > 0
#define CONSTR_ARGS(list)  const std::vector<Literal>& list, unsigned int number
#define INIT_FOR_VERBOSE()  _number(number),
#else
#define CONSTR_ARGS(list)  const std::vector<Literal>& list
#define INIT_FOR_VERBOSE()  
#endif


#include "BasicClause.hh"
#include "ConstAssignClause.hh"
#include "OneWatchedClause.hh"
//#include "BasicClauseWatched.hh"


#endif //CLAUSE_HH
