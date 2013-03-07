#ifndef BASICCLAUSE_HH
#define BASICCLAUSE_HH

#include <vector>
#include "Literal.hh"
#include <set>
#include <stack>
#include <cstdlib> // pour exit()

#ifndef INLINED_CLAUSE
  #include "Clause.hh"
  #ifndef VIRTUAL
    #define VIRTUAL virtual
  #endif
  #ifndef HERITED_CLAUSE
    #define HERITED_CLAUSE  : public Clause
  #endif
#else
  #ifndef VIRTUAL
    #define VIRTUAL 
  #endif
  #ifndef HERITED_CLAUSE
    #define HERITED_CLAUSE 
  #endif
#endif

class BasicClause HERITED_CLAUSE
{
public:
    BasicClause(const std::vector<Literal>& list);
    
    VIRTUAL void setLitFalse(const Literal& l);
    VIRTUAL void setLitTrue(const Literal& l);

    VIRTUAL void freeLitFalse(const Literal &l);
    VIRTUAL void freeLitTrue(const Literal &l);
    
    VIRTUAL size_t freeSize (void) const;
    VIRTUAL Literal chooseFree(void) const;
    VIRTUAL bool satisfied(void) const;

    VIRTUAL ~BasicClause();

protected:
    bool _satisfied;
    std::stack<Literal> _assigned;
    std::set<Literal> _free;
};


// corps des fonctions de la classe (toutes inlines)
#include "BasicClause.inline.hh"

#endif //BASICCLAUSE_HH
