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
    VIRTUAL void setLitFalse(Literal& l, SatProblem& sp);
    VIRTUAL void setLitTrue(const Literal& l, SatProblem& sp);
    
    //VIRTUAL void freeVar(const unsigned int varId);
    VIRTUAL void freeLitFalse(const Literal &l, SatProblem& sp);
    VIRTUAL void freeLitTrue(const Literal &l, SatProblem& sp);
    
    VIRTUAL size_t freeSize (SatProblem& sp);
    //VIRTUAL size_t assignedSize(void) const;
    VIRTUAL Literal chooseFree(SatProblem& sp) const;
    VIRTUAL bool satisfied(SatProblem& sp) const;
    
    VIRTUAL ~BasicClauseWatched();


protected:
    Literal _watched1, _watched2;
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
