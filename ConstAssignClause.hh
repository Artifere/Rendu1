#ifndef CONSTASSIGNCLAUSE_HH
#define CONSTASSIGNCLAUSE_HH

#include <vector>
#include "Literal.hh"

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

class ConstAssignClause HERITED_CLAUSE
{
    // apparamment tout doit être réécrit ici pour l'héritage avec polymorphisme
public:
    ConstAssignClause(const std::vector<Literal>& list);
    
    //virtual void setVar(const Literal &l);
    VIRTUAL void setLitFalse(const Literal& l);
    VIRTUAL void setLitTrue(const Literal& l);

    //virtual void freeVar(const unsigned int varId);
    VIRTUAL void freeLitFalse(const Literal &l);
    VIRTUAL void freeLitTrue(const Literal &l);
    
    VIRTUAL size_t freeSize (void) const;
    VIRTUAL size_t assignedSize(void) const;
    VIRTUAL Literal chooseFree(void) const;
    VIRTUAL bool satisfied(void) const;

    VIRTUAL ~ConstAssignClause();


protected:
    inline unsigned int hashOfLit(const Literal& l) const { return (l.var()<<1) + (l.pos() ? 1 : 0); };
    inline unsigned int hashOfInvertLit(const Literal& l) const { return (l.var()<<1) + (l.pos() ? 0 : 1); };
    inline Literal LitOfHash(const unsigned int& i) const { return Literal(i>>1, (i &1)); };

    //std::set<Literal> _initLit;
    //unsigned int _initHash;
    unsigned int _currentHash;
    unsigned int _satisfied;
    unsigned int _numOfFree;
};


// corps des fonctions de la classe (toutes inlines)
#include "ConstAssignClause.inline.hh"

#endif //CONSTASSIGNCLAUSE_HH
