#include "ExprTree.hh"
#include <iostream>
#include "Parser.hh"

static inline clause
doClause(const literal& l1)
{
    clause tmp;
    tmp.push_back(l1);
    return tmp;
}
static inline clause
doClause(const literal& l1, const literal& l2)
{
    clause tmp;
    tmp.push_back(l1);
    tmp.push_back(l2);
    return tmp;
}
static inline clause
doClause(const literal& l1, const literal& l2, const literal& l3)
{
    clause tmp;
    tmp.push_back(l1);
    tmp.push_back(l2);
    tmp.push_back(l3);
    return tmp;
}

#define CLAUSE1(a) doClause(literal a)
#define CLAUSE2(a,b) doClause(literal a, literal b)
#define CLAUSE3(a,b,c) doClause(literal a, literal b, literal c)

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
    
    literal lastNode = res->getCNF(listClause);
    // ajoute la dernière clause : celle qui dit que la formule est vraie
    listClause.push_back(doClause(lastNode));
    
    std::swap(ExprTree::varNumbers, varNumbers);
    std::swap(lastUsedId, ExprTree::lastUsedId);
    
    return lastUsedId;
}




#if 0
literal Imply::getCNF(std::vector<clause>& cnf) const
{
    literal left = c1->getCNF(cnf);
    literal right = c2->getCNF(cnf);
    literal self = literal(++lastUsedId, true);
    cnf.push_back(doClause(invert(self), invert(left), right));
    cnf.push_back(doClause(self, left));
    cnf.push_back(doClause(self, invert(right)));
    /*
    cnf.push_back(CLAUSE3((self,false), (left,false), (right,true)));
    cnf.push_back(CLAUSE2((self,true),  (left,true)));
    cnf.push_back(CLAUSE2((self,true),  (right,false)));
    */
    return self;
}
#endif

literal Or::getCNF(std::vector<clause>& cnf) const
{
    literal left = c1->getCNF(cnf);
    literal right = c2->getCNF(cnf);
    literal self = literal(++lastUsedId, true);
    cnf.push_back(doClause(invert(self), left, right));
    cnf.push_back(doClause(self, invert(left)));
    cnf.push_back(doClause(self, invert(right)));
    /*
    cnf.push_back(CLAUSE3((self,false), (left,true), (right,true)));
    cnf.push_back(CLAUSE2((self,true), (left,false)));
    cnf.push_back(CLAUSE2((self,true), (right,false)));
    */
    return self;
}

literal And::getCNF(std::vector<clause>& cnf) const
{
    literal left = c1->getCNF(cnf);
    literal right = c2->getCNF(cnf);
    literal self = literal(++lastUsedId, true);
    cnf.push_back(doClause(self, invert(left), invert(right)));
    cnf.push_back(doClause(invert(self), left));
    cnf.push_back(doClause(invert(self), right));
    /*
    cnf.push_back(CLAUSE3((self,true), (left,false), (right,false)));
    cnf.push_back(CLAUSE2((self,false), (left,true)));
    cnf.push_back(CLAUSE2((self,false), (right,true)));
    */
    return self;
}

literal Not::getCNF(std::vector<clause>& cnf) const
{
    literal fils = c1->getCNF(cnf);
    literal self = literal(++lastUsedId, true);
    cnf.push_back(doClause(invert(self), invert(fils)));
    cnf.push_back(doClause(self, fils));
    /*
    cnf.push_back(CLAUSE2((self,false), (fils,false)));
    cnf.push_back(CLAUSE2((self,true), (fils,true)));
    */
    return self;
}

literal Val::getCNF(std::vector<clause>& cnf) const
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

/*
literal Imply::getSmallCNF(std::vector<clause>& cnf) const
{
    return literal(0,true);
}
*/
literal Or::getSmallCNF(std::vector<clause>& cnf) const
{
    return literal(0,true);
}
literal And::getSmallCNF(std::vector<clause>& cnf) const
{
    return literal(0,true);
}
literal Not::getSmallCNF(std::vector<clause>& cnf) const
{
    return literal(0,true);
}
literal Val::getSmallCNF(std::vector<clause>& cnf) const
{
    return literal(0,true);
}


