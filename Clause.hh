#include "Literal.hh"


Class Clause
{
  public:
    void setLitFalse(const Literal& l);
    void setLitTrue(const Literal& l);
    void freeVar(const unsigned int varId);
    bool freeEmpty (void) const;
    Literal chooseFree(void) const;
}
