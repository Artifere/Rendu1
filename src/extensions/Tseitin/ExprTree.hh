#ifndef EXPRTREE_HH
#define EXPRTREE_HH


#include <memory>
#include <istream>
#include <vector>
#include <utility>


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
    virtual literal getSmallCNF(std::vector<clause>& cnf) const = 0;
    virtual ExprTree* inversion() const = 0;
    
    virtual void addCNF(std::vector<clause>& cnf) const = 0;
    virtual void addCNF_readClause(std::vector<clause>& cnf, clause& cl) const = 0;
    virtual literal addCNF_readLiteral(std::vector<clause>& cnf) const = 0;
};


unsigned ClauseTseitin(std::istream& in, std::vector<clause>& listClause, std::vector<std::pair<std::string,unsigned> >& varNumbers);



class And : public ExprTree
{
public:
    ExprTree * c1;
    ExprTree * c2;

    inline And(ExprTree * a1, ExprTree * a2) : c1(a1), c2(a2) { };

    inline ~And() { delete c1; delete c2; };

    unsigned getCNF(std::vector<clause>& cnf) const;
    literal getSmallCNF(std::vector<clause>& cnf) const;
    
    ExprTree* inversion() const;
    
    void addCNF(std::vector<clause>& cnf) const;
    void addCNF_readClause(std::vector<clause>& cnf, clause& cl) const;
    literal addCNF_readLiteral(std::vector<clause>& cnf) const;
};



class Or : public ExprTree
{
public:
    ExprTree * c1;
    ExprTree * c2;

    inline Or(ExprTree * a1, ExprTree * a2) : c1(a1), c2(a2) { };

    inline ~Or() { delete c1; delete c2; };

    unsigned getCNF(std::vector<clause>& cnf) const;
    literal getSmallCNF(std::vector<clause>& cnf) const;

    ExprTree* inversion() const;
    
    void addCNF(std::vector<clause>& cnf) const;
    void addCNF_readClause(std::vector<clause>& cnf, clause& cl) const;
    literal addCNF_readLiteral(std::vector<clause>& cnf) const;
};


class Val : public ExprTree
{
public:
    std::string _name;
    bool _val;

    inline Val(const std::string& name, bool val) : _name(name), _val(val) { }
    
    inline ~Val() { }

    unsigned getCNF(std::vector<clause>& cnf) const;
    literal getSmallCNF(std::vector<clause>& cnf) const;

    ExprTree* inversion() const;
    
    void addCNF(std::vector<clause>& cnf) const;
    void addCNF_readClause(std::vector<clause>& cnf, clause& cl) const;
    literal addCNF_readLiteral(std::vector<clause>& cnf) const;
    
    unsigned getVarId() const;
};



#endif//EXPRTREE_HH
