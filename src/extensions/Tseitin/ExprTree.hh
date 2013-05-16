#ifndef EXPRTREE_HH
#define EXPRTREE_HH


#include <memory>
#include <istream>
#include <vector>
#include <utility>



typedef std::pair<unsigned, bool> literal;
typedef std::vector<literal> clause;


static inline literal invert(literal lit)
{
    lit.second = ! lit.second;
    return lit;
}



class ExprTree
{
public:
    static unsigned lastUsedId;
    static std::vector<std::pair<std::string,unsigned> > varNumbers; // association nom de variable/numéro
   
    virtual inline ~ExprTree() { }

    virtual ExprTree* inversion() const = 0;
    virtual void print(std::ostream& out) const = 0;
    
    // version de base de la transformation de tseitin
    virtual unsigned getCNF(std::vector<clause>& cnf) const = 0;
    
    // version un peu plus intelligente de Tseitin
    virtual void addCNF(std::vector<clause>& cnf) const = 0;
    virtual literal addCNF_readLiteral(std::vector<clause>& cnf) const = 0;
    // ces deux fonctions sont à la base de l'optimisation faie par cette version :
    // on cherche à augmenter l'arité des noeuds And et OR pour éviter d'avoir à rajouter des variables
    virtual inline void addCNF_readOr(std::vector<clause>& cnf, std::vector<literal>& listLit) const
    {
        listLit.push_back(addCNF_readLiteral(cnf));
    }
    virtual inline void addCNF_readAnd(std::vector<clause>& cnf, std::vector<literal>& listLit) const
    {
        listLit.push_back(addCNF_readLiteral(cnf));
    }
};

// permet d'afficher une ExprTree directment dans un flux
inline std::ostream& operator<<(std::ostream& out, ExprTree* e)
{
    e->print(out);
    return out;
}



class And : public ExprTree
{
protected:
    ExprTree * c1;
    ExprTree * c2;

public:
    inline And(ExprTree * a1, ExprTree * a2) : c1(a1), c2(a2)
    {
    }
    inline ~And()
    {
        delete c1;
        delete c2;
    }
    
    ExprTree* inversion() const;
    void print(std::ostream& out) const;    

    unsigned getCNF(std::vector<clause>& cnf) const;
    
    void addCNF(std::vector<clause>& cnf) const;
    literal addCNF_readLiteral(std::vector<clause>& cnf) const;
    // on ne spécialise que cette fonction : readOR est identique à celle par défaut
    void addCNF_readAnd(std::vector<clause>& cnf, std::vector<literal>& listLit) const
    {
        c1->addCNF_readAnd(cnf,listLit);
        c2->addCNF_readAnd(cnf,listLit);
    }
};



class Or : public ExprTree
{
protected:
    ExprTree * c1;
    ExprTree * c2;

public:
    inline Or(ExprTree * a1, ExprTree * a2) : c1(a1), c2(a2)
    {
    }
    inline ~Or()
    {
        delete c1;
        delete c2;
    }

    ExprTree* inversion() const;
    void print(std::ostream& out) const;
    
    unsigned getCNF(std::vector<clause>& cnf) const;
    
    void addCNF(std::vector<clause>& cnf) const;
    literal addCNF_readLiteral(std::vector<clause>& cnf) const;
    // on ne spécialise que cette fonction : readAnd est identique à celle par défaut
    inline void addCNF_readOr(std::vector<clause>& cnf, std::vector<literal>& listLit) const
    {
        c1->addCNF_readOr(cnf,listLit);
        c2->addCNF_readOr(cnf,listLit);
    }
};



class Val : public ExprTree
{
protected:
    std::string _name;
    bool _val;
    
    // cherche si la variable à déjà un numéro attribué
    // et lui attribue un numéro si non
    unsigned getVarId() const;

public:
    inline Val(const std::string& name, bool val) : _name(name), _val(val)
    {
    }
    inline ~Val()
    {
    }

    ExprTree* inversion() const;
    void print(std::ostream& out) const;

    unsigned getCNF(std::vector<clause>& cnf) const;
    
    void addCNF(std::vector<clause>& cnf) const;
    literal addCNF_readLiteral(std::vector<clause>& cnf) const;
};



#endif//EXPRTREE_HH
