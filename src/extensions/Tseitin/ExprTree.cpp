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

static inline literal invert(literal lit)
{
    lit.second = ! lit.second;
    return lit;
}



unsigned ExprTree::lastUsedId;
std::vector<std::pair<std::string,unsigned> > ExprTree::varNumbers;


unsigned ClauseTseitin(std::istream& in, std::vector<clause>& listClause, std::vector<std::pair<std::string,unsigned> >& varNumbers)
{
    unsigned lastUsedId = ExprTree::lastUsedId;
    ExprTree::lastUsedId = 0;
    std::swap(ExprTree::varNumbers, varNumbers);

    ParserExprTree parser(in);
    ExprTree * res = parser.parseExpr();
    
    //res->print(std::cout); std::cout << std::endl;
    
    //unsigned lastNode = res->getCNF(listClause);
    // ajoute la dernière clause : celle qui dit que la formule est vraie
    //ADD_CLAUSE1(listClause, (lastNode,true));
    // 
    /*
    literal lastNode = res->getSmallCNF(listClause);
    addClause(listClause, lastNode);
    */
    res->addCNF(listClause);
    
    std::swap(ExprTree::varNumbers, varNumbers);
    std::swap(lastUsedId, ExprTree::lastUsedId);
    
    return lastUsedId;
}







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
/*
unsigned Not::getCNF(std::vector<clause>& cnf) const
{
    unsigned fils = c1->getCNF(cnf);
    unsigned self = ++lastUsedId;
    ADD_CLAUSE2(cnf, (self,false), (fils,false));
    ADD_CLAUSE2(cnf, (self,true), (fils,true));
    return self;
}
*/
unsigned Val::getCNF(std::vector<clause>& cnf) const
{
    (void)cnf; // ignore l'argument sans causer de warning
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
    if (_val)
    {
        return id;
    }
    else
    {
        unsigned notid = ++lastUsedId;
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


    
void Or::addCNF(std::vector<clause>& cnf) const
{
    clause cl;
    c1->addCNF_readClause(cnf, cl);
    c2->addCNF_readClause(cnf, cl);
    cnf.push_back(cl);
}
void And::addCNF(std::vector<clause>& cnf) const
{
    c1->addCNF(cnf);
    c2->addCNF(cnf);
}
void Val::addCNF(std::vector<clause>& cnf) const
{
    literal lit(getVarId(), _val);
    clause cl;
    cl.push_back(lit);
    cnf.push_back(cl);
}



void And::addCNF_readClause(std::vector<clause>& cnf, clause& cl) const
{
    cl.push_back(addCNF_readLiteral(cnf));
}
void Or::addCNF_readClause(std::vector<clause>& cnf, clause& cl) const
{
    c1->addCNF_readClause(cnf, cl);
    c2->addCNF_readClause(cnf, cl);
}
void Val::addCNF_readClause(std::vector<clause>& cnf, clause& cl) const
{
    cl.push_back(addCNF_readLiteral(cnf));
}


literal And::addCNF_readLiteral(std::vector<clause>& cnf) const
{
    literal left = c1->addCNF_readLiteral(cnf);
    literal right = c2->addCNF_readLiteral(cnf);
    literal self = literal(++lastUsedId, true);
    addClause(cnf, self, invert(left), invert(right));
    addClause(cnf, invert(self), left);
    addClause(cnf, invert(self), right);
    
    return self;
}
literal Or::addCNF_readLiteral(std::vector<clause>& cnf) const
{
    clause cl;
    c1->addCNF_readClause(cnf, cl);
    c2->addCNF_readClause(cnf, cl);
    literal self(++ lastUsedId, true);
    // ajoute l'équivalent de (invert(self), left, right)
    cl.push_back(invert(self));
    cnf.push_back(cl);
    cl.pop_back();
    // ajoute les équivalents de (self, invert(left)) et (self, invert(right))
    clause::const_iterator it;
    for (it = cl.begin(); it != cl.end(); ++it)
        addClause(cnf, self, invert(*it));
    // renvoie le littéral correspondant au noeud
    return self;
}
literal Val::addCNF_readLiteral(std::vector<clause>& cnf) const
{
    (void)cnf; // évite de générer un warning 'Unused argument' à la compilation
    return literal(getVarId(), _val);
}









literal Or::getSmallCNF(std::vector<clause>& cnf) const
{
    literal left = c1->getSmallCNF(cnf);
    literal right = c2->getSmallCNF(cnf);
    literal self = literal(++lastUsedId, true);
    addClause(cnf, invert(self), left, right);
    addClause(cnf, self, invert(left));
    addClause(cnf, self, invert(right));
    return self;
}

literal And::getSmallCNF(std::vector<clause>& cnf) const
{
    literal left = c1->getSmallCNF(cnf);
    literal right = c2->getSmallCNF(cnf);
    literal self = literal(++lastUsedId, true);
    addClause(cnf, self, invert(left), invert(right));
    addClause(cnf, invert(self), left);
    addClause(cnf, invert(self), right);
    return self;
}
/*
literal Not::getSmallCNF(std::vector<clause>& cnf) const
{
    literal fils = c1->getSmallCNF(cnf);
    return invert(fils);
}
*/
literal Val::getSmallCNF(std::vector<clause>& cnf) const
{
    (void)cnf; // ignore l'argument sans causer de warning
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
    return literal(id, true);
}

