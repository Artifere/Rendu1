#ifndef EXPRTREE_HH
#define EXPRTREE_HH


#include <memory>
#include <istream>
#include <vector>
#include <utility>
#include <cassert>



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

    // étiquettes possibles des noeuds internes
    typedef enum {
        AND, OR, XOR
    } type;

public:
    virtual inline ~ExprTree() { }

    // inverse l'expression (fait l'opération  expr <- ~ expr)
    virtual void inversion() = 0;

    // affiche l'expression bien parenthesée
    virtual void print(std::ostream& out) const = 0;
    
    // transformation de Tseitin
    virtual void addCNF(std::vector<clause>& cnf) const = 0;

    // fait remonter tous les noeuds de même type au même niveau
    // (exemple :  a /\ (b /\ c) devient a /\ b /\ c)
    // son utilisation n'est pas obligatoire, mais elle peut réduire le nombre de variables intermédiaires
    virtual void simplify() = 0;


    // utilisé par addCNF : crée un litéral équivalent à la valeur du noeud
    // coeur de la transformation de tseitin
    virtual literal getLiteral(std::vector<clause>& cnf) const = 0;
    
    // utilisé en interne par simplify :
    // test si le noeud est de type t
    // si c'est le cas, remonte récursivement tous ses enfants dans dest, et renvoie true
    // sinon renvoie false
    virtual bool remonte(std::vector<ExprTree*>& dest, ExprTree::type t) = 0;
};





// permet d'afficher une ExprTree directment dans un flux
inline std::ostream& operator<<(std::ostream& out, ExprTree* e)
{
    e->print(out);
    return out;
}





class NodeExpr : public ExprTree
{
protected:
    typedef std::vector<ExprTree*> fils;

    ExprTree::type _type;
    fils _args; // doit toujours être de taille au moins 2

public:
    inline NodeExpr(ExprTree::type t, const std::vector<ExprTree*>& args) : _type(t), _args(args)
    {
        assert(args.size() >= 2);
    }
    inline NodeExpr(ExprTree::type t, ExprTree* arg1, ExprTree* arg2) : _type(t), _args(2, NULL)
    {
        _args[0] = arg1;
        _args[1] = arg2;
    }

    virtual inline ~NodeExpr()
    {
        for(fils::const_iterator it = _args.begin(); it != _args.end(); ++it)
            delete *it;
    }

    // inverse l'expression (fait l'opération  expr <- ~ expr)
    void inversion();

    // affiche l'expression bien parenthesée
    void print(std::ostream& out) const;
    
    // transformation de Tseitin
    void addCNF(std::vector<clause>& cnf) const;

    // fait remonter tous les noeuds de même type au même niveau
    // (exemple :  a /\ (b /\ c) devient a /\ b /\ c)
    // son utilisation n'est pas obligatoire, mais elle peut réduire le nombre de variables intermédiaires
    void simplify();


    // utilisé par addCNF : crée un litéral équivalent à la valeur du noeud
    // coeur de la transformation de tseitin
    literal getLiteral(std::vector<clause>& cnf) const;
    
    // utilisé en interne par simplify :
    // test si le noeud est de type t
    // si c'est le cas, déplace tous ses enfants dans dest, et renvoie true
    // sinon renvoie false
    bool remonte(std::vector<ExprTree*>& dest, ExprTree::type t);
};





class ValExpr : public ExprTree
{
protected:
    std::string _name;
    bool _val;
    
    // cherche si la variable à déjà un numéro attribué
    // et lui attribue un numéro si non
    unsigned getVarId() const;

public:
    inline ValExpr(const std::string& name, bool val) : _name(name), _val(val) { }

    inline ~ValExpr() { }

    // inverse l'expression (fait l'opération  expr <- ~ expr)
    void inversion();

    // affiche l'expression bien parenthesée
    void print(std::ostream& out) const;
    
    // transformation de Tseitin
    void addCNF(std::vector<clause>& cnf) const;

    // fait remonter tous les noeuds de même type au même niveau
    // (exemple :  a /\ (b /\ c) devient a /\ b /\ c)
    // son utilisation n'est pas obligatoire, mais elle peut réduire le nombre de variables intermédiaires
    void simplify();


    // utilisé par addCNF : crée un litéral équivalent à la valeur du noeud
    // coeur de la transformation de tseitin
    literal getLiteral(std::vector<clause>& cnf) const;
    
    // utilisé en interne par simplify :
    // test si le noeud est de type t
    // si c'est le cas, déplace tous ses enfants dans dest, et renvoie true
    // sinon renvoie false
    bool remonte(std::vector<ExprTree*>& dest, ExprTree::type t);
    
};


#endif//EXPRTREE_HH
