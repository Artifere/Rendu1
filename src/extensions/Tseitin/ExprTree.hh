#ifndef EXPRTREE_HH
#define EXPRTREE_HH


#include <memory>
#include <istream>
#include <vector>
#include <utility>
#include "Parser.hh"


//typedef std::unique_ptr<ExprTree> expr_ptr;

typedef std::pair<unsigned, bool> literal;
typedef std::vector<literal> clause;



class ExprTree
{
public:
   static unsigned lastUsedId;
   static std::vector<std::pair<std::string,unsigned> > varNumbers; // association nom de variable/numéro
   
   virtual inline ~ExprTree() { };
   
   virtual unsigned getCNF(std::vector<clause>& cnf) const = 0;
};


unsigned ClauseTseitin(std::istream& in, std::vector<clause>& listClause, std::vector<std::pair<std::string,unsigned> >& varNumbers);


class Imply : public ExprTree
{
public:
    ExprTree * c1;
    ExprTree * c2;

    inline Imply(ExprTree * a1, ExprTree * a2) : c1(a1), c2(a2) { };

    inline ~Imply() { delete c1; delete c2; };

    virtual unsigned getCNF(std::vector<clause>& cnf) const;
};



class And : public ExprTree
{
public:
    ExprTree * c1;
    ExprTree * c2;

    inline And(ExprTree * a1, ExprTree * a2) : c1(a1), c2(a2) { };

    inline ~And() { delete c1; delete c2; };

    virtual unsigned getCNF(std::vector<clause>& cnf) const;
};



class Or : public ExprTree
{
public:
    ExprTree * c1;
    ExprTree * c2;

    inline Or(ExprTree * a1, ExprTree * a2) : c1(a1), c2(a2) { };

    inline ~Or() { delete c1; delete c2; };

    virtual unsigned getCNF(std::vector<clause>& cnf) const;
};



class Not : public ExprTree
{
public:
    ExprTree * c1;

    inline Not(ExprTree * a1) : c1(a1) { };

    inline ~Not() { delete c1; };

    virtual unsigned getCNF(std::vector<clause>& cnf) const;
};



class Val : public ExprTree
{
public:
    bool val;
    unsigned number;

    inline Val(unsigned number, bool val) : val(val), number(number) { };
    
    inline ~Val() { };

    virtual unsigned getCNF(std::vector<clause>& cnf) const;
};



#endif//EXPRTREE_HH
