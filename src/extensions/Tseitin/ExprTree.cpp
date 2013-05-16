#include "ExprTree.hh"
#include <iostream>
#include "Parser.hh"

static inline void
addClause(std::vector<clause>& cnf, const literal& l1)
{
    clause tmp;
    tmp.push_back(l1);
    cnf.push_back(tmp);
}
static inline void
addClause(std::vector<clause>& cnf, const literal& l1, const literal& l2)
{
    clause tmp;
    tmp.push_back(l1);
    tmp.push_back(l2);
    cnf.push_back(tmp);
}
static inline void
addClause(std::vector<clause>& cnf, const literal& l1, const literal& l2, const literal& l3)
{
    clause tmp;
    tmp.push_back(l1);
    tmp.push_back(l2);
    tmp.push_back(l3);
    cnf.push_back(tmp);
}

#define ADD_CLAUSE1(cnf, a) addClause(cnf, literal a)
#define ADD_CLAUSE2(cnf, a,b) addClause(cnf, literal a, literal b)
#define ADD_CLAUSE3(cnf, a,b,c) addClause(cnf, literal a, literal b, literal c)




unsigned ExprTree::lastUsedId;
std::vector<std::pair<std::string,unsigned> > ExprTree::varNumbers;





unsigned Or::getCNF(std::vector<clause>& cnf) const
{
    unsigned left = c1->getCNF(cnf);
    unsigned right = c2->getCNF(cnf);
    unsigned self = ++lastUsedId;
    ADD_CLAUSE3(cnf, (self,false), (left,true), (right,true));
    ADD_CLAUSE2(cnf, (self,true), (left,false));
    ADD_CLAUSE2(cnf, (self,true), (right,false));
    return self;
}

unsigned And::getCNF(std::vector<clause>& cnf) const
{
    unsigned left = c1->getCNF(cnf);
    unsigned right = c2->getCNF(cnf);
    unsigned self = ++lastUsedId;
    ADD_CLAUSE3(cnf, (self,true), (left,false), (right,false));
    ADD_CLAUSE2(cnf, (self,false), (left,true));
    ADD_CLAUSE2(cnf, (self,false), (right,true));
    return self;
}

unsigned Val::getCNF(std::vector<clause>& cnf) const
{
    unsigned id = getVarId();
    if (_val)
    {
        return id;
    }
    else
    {
        unsigned notid = ++ lastUsedId;
        ADD_CLAUSE2(cnf, (id,false), (notid,false));
        ADD_CLAUSE2(cnf, (id,true), (notid,true));
        return notid;
    }
}





ExprTree* Or::inversion() const
{
    ExprTree* nc1 = c1->inversion();
    ExprTree* nc2 = c2->inversion();
    return new And(nc1, nc2);
}

ExprTree* And::inversion() const
{
    ExprTree* nc1 = c1->inversion();
    ExprTree* nc2 = c2->inversion();
    return new Or(nc1, nc2);
}

ExprTree* Val::inversion() const
{
    return new Val(_name, !_val);
}





void Or::print(std::ostream& out) const
{
    out << "(";
    c1->print(out);
    out << " \\/ ";
    c2->print(out);
    out << ")";
}

void And::print(std::ostream& out) const
{
    out << "(";
    c1->print(out);
    out << " /\\ ";
    c2->print(out);
    out << ")";
}

void Val::print(std::ostream& out) const
{
    out << (_val ? ' ' : '~') << _name;
}





unsigned Val::getVarId() const
{
    unsigned id;
    // cherche si la variable à déjà un numéro attribué
    // et lui attribue un numéro si non
    std::vector<std::pair<std::string,unsigned> >::const_iterator it;
    for(it = ExprTree::varNumbers.begin(); it != ExprTree::varNumbers.end(); ++it)
    {
        if (it->first == _name)
            break;
    }
    if (it == ExprTree::varNumbers.end()) {
        id = ++ ExprTree::lastUsedId;
        ExprTree::varNumbers.push_back(std::pair<std::string,unsigned>(_name,id));
    } else {
        id = it->second;
    }
    return id;
}




    
void And::addCNF(std::vector<clause>& cnf) const
{
    c1->addCNF(cnf);
    c2->addCNF(cnf);
}

void Or::addCNF(std::vector<clause>& cnf) const
{
    clause cl;
    c1->addCNF_readOr(cnf, cl);
    c2->addCNF_readOr(cnf, cl);
    cnf.push_back(cl);
}

void Val::addCNF(std::vector<clause>& cnf) const
{
    addClause(cnf, literal(getVarId(), _val));
}



literal And::addCNF_readLiteral(std::vector<clause>& cnf) const
{
    // crée un litéral à ajouter à listLit
    literal self = literal(++lastUsedId, true);
    
    // lie self à la valeur logique du noeud par transformation se Tseitin :

    // récupère tous les litéraux provennant de sous-noeuds And :
    // on évite ainsi de créer trop de variables et de clauses
    // (on cherche à appliquer la transformation de Tseitin sur une arité plus grande que 2 pour ce noeud And)
    std::vector<literal> subLit;
    c1->addCNF_readAnd(cnf, subLit);
    c2->addCNF_readAnd(cnf, subLit);

    // ajoute les équivalents de (invert(self), left) et (invert(self), right) avec une arité >= 2
    clause::iterator it;
    for (it = subLit.begin(); it != subLit.end(); ++it)
        addClause(cnf, invert(self), *it);

    // ajoute l'équivalent de (self, non(left), non(right)) avec une arité >= 2
    for (it = subLit.begin(); it != subLit.end(); ++it)
        it->second = ! it->second; // inverse les litéraux
    subLit.push_back(self);
    cnf.push_back(subLit);
    
    return self;
}

literal Or::addCNF_readLiteral(std::vector<clause>& cnf) const
{
    // crée un litéral à ajouter à listLit
    literal self = literal(++lastUsedId, true);
    
    // lie self à la valeur logique du noeud par transformation se Tseitin :

    // récupère tous les litéraux provennant de sous-noeuds And :
    // on évite ainsi de créer trop de variables et de clauses
    // (on cherche à appliquer la transformation de Tseitin sur une arité plus grande que 2 pour ce noeud And)
    std::vector<literal> subLit;
    c1->addCNF_readOr(cnf, subLit);
    c2->addCNF_readOr(cnf, subLit);

    // ajoute les équivalents de (self, invert(left)) et (self, invert(right)) avec une arité >= 2
    clause::const_iterator it;
    for (it = subLit.begin(); it != subLit.end(); ++it)
        addClause(cnf, self, invert(*it));

    // ajoute l'équivalent de (invert(self), left, right) avec une arité >= 2
    subLit.push_back(invert(self));
    cnf.push_back(subLit);
    
    return self;
}

literal Val::addCNF_readLiteral(std::vector<clause>& cnf) const
{
    (void)cnf; // évite de générer un 'warning : unused parameter' à la compilation
    return literal(getVarId(), _val);
}




