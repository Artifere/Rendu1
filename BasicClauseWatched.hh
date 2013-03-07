#ifndef BASICCLAUSEWATCHED_HH
#define BASICCLAUSEWATCHED_HH

#include <vector>
#include "Literal.hh"
#include <set>
#include <stack>
#include "satSolverWatched.hh"

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

class SatProblem;

class BasicClauseWatched HERITED_CLAUSE
{
    friend class SatProblem;
public:

    BasicClauseWatched(const std::vector<Literal>& list);
    
    //VIRTUAL void setVar(const Literal &l);
    VIRTUAL void setLitFalse(const Literal& l, SatProblem& sp);
    VIRTUAL void setLitTrue(const Literal& l, SatProblem& sp);
    
   
    VIRTUAL unsigned int freeSize (const SatProblem& sp) const;
    VIRTUAL Literal chooseFree(const SatProblem& sp) const;
    VIRTUAL bool satisfied(const SatProblem& sp) const;
    
    VIRTUAL ~BasicClauseWatched();


protected:
    std::vector<Literal> _literals;

    
    
    
    /*
    bool _satisfied;
    std::stack<Literal> _assigned;
    std::set<Literal> _free;
    */
};


// corps des fonctions de la classe (toutes inlines)
#include "BasicClauseWatched.inline.hh"

#endif //BASICCLAUSEWATCHED_HH
