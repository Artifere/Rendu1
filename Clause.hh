#include "Literal.hh"


Class Clause
{
  public:
    virtual void setLitFalse(const Literal& l);
    virtual void setLitTrue(const Literal& l);
    virtual void freeVar(const unsigned int varId);
    virtual bool freeEmpty (void) const;
    virtual Literal chooseFree(void) const;
};
