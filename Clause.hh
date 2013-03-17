#ifndef CLAUSE_HH
#define CLAUSE_HH

#include "Literal.hh"


#ifndef INLINED_CLAUSE
class Clause
{
public:

    // return true iff the literal l is no longer watched by the clause
    // (so the link between the clause and the variable l.var() should be removed)
    virtual bool setLitFalse(const Literal& l)=0;
    virtual bool setLitTrue(const Literal& l)=0;

    virtual void freeLitFalse(const Literal &l)=0;
    virtual void freeLitTrue(const Literal &l)=0;

    virtual size_t freeSize (void) const=0;
    virtual Literal chooseFree(void) const=0;
    virtual bool satisfied(void) const=0;

    virtual ~Clause() {}
};
#endif



#ifndef INLINED_CLAUSE
    #define VIRTUAL virtual
    #define HERITED_CLAUSE  : public Clause
#else
    #define VIRTUAL 
    #define HERITED_CLAUSE 
#endif


#if VERBOSE > 1
#define CONSTR_ARGS(list)  std::vector<Literal>& list, const unsigned int number
#define INIT_FOR_VERBOSE() _number(number),
#define CALL_CONSTR(list)  list, number
#else
#define CONSTR_ARGS(list)  std::vector<Literal>& list
#define INIT_FOR_VERBOSE() 
#define CALL_CONSTR(list)   list
#endif


#include "BasicClause.hh"
#include "ConstAssignClause.hh"
#include "OneWatchedClause.hh"
#include "BasicClauseWatched.hh"
#include "SmartClause.hh"
#include "SmartClauseWatched.hh"

#endif //CLAUSE_HH
