#ifndef CONSTASSIGNCLAUSE_HH
#define CONSTASSIGNCLAUSE_HH



#include "Clause.hh"
#include <stack>
#include <set>


class ConstAssignClause : public Clause
{
    // apparamment tout doit être réécrit ici pour l'héritage avec polymorphisme
public:
    ConstAssignClause(const std::vector<Literal>& list);
    
    virtual void setVar(const Literal &l);
    virtual void setLitFalse(const Literal& l);
    virtual void setLitTrue(const Literal& l);

    virtual void freeVar(const unsigned int varId);
    virtual void freeLitFalse(const Literal &l);
    virtual void freeLitTrue(const Literal &l);
    
    virtual size_t freeSize (void) const;
    virtual size_t assignedSize(void) const;
    virtual Literal chooseFree(void) const;
    virtual bool satisfied(void) const;

    virtual ~ConstAssignClause();


protected:
    inline unsigned int hashOfLit(const Literal& l) const { return 2*l.var() + (l.pos() ? 1 : 0); };
    inline unsigned int hashOfInvertLit(const Literal& l) const { return 2*l.var() + (l.pos() ? 0 : 1); };
    inline Literal LitOfHash(const unsigned int& i) const { return Literal(i/2, (i % 2 != 0)); };

    std::set<Literal> _initLit;
    unsigned int _initHash;
    unsigned int _currentHash;
    unsigned int _satisfied;
    unsigned int _numOfFree;
};


#endif //CONSTASSIGNCLAUSE_HH
