#include "ExprTree.hh"
#include <iostream>

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



unsigned ExprTree::lastUsedId;
std::vector<std::pair<std::string,unsigned> > ExprTree::varNumbers;



ExprTree* parseImply(std::istream& in);
ExprTree* parseOr(std::istream& in);
ExprTree* parseAnd(std::istream& in);
ExprTree* parseNot(std::istream& in);
ExprTree* parseVal(std::istream& in);
ExprTree* parseExpr(std::istream& in);



unsigned ClauseTseitin(std::istream& in, std::vector<clause>& listClause, std::vector<std::pair<std::string,unsigned> >& varNumbers)
{
    unsigned lastUsedId = ExprTree::lastUsedId;
    ExprTree::lastUsedId = 0;
    std::swap(ExprTree::varNumbers, varNumbers);

    Token::nextToken(in);
    ExprTree * res = parseExpr(in);
    
    unsigned lastNode = res->getCNF(listClause);
    // ajoute la dernière clause : celle qui dit que la formule est vraie
    listClause.push_back(CLAUSE1((lastNode,true)));
    
    std::swap(ExprTree::varNumbers, varNumbers);
    std::swap(lastUsedId, ExprTree::lastUsedId);
    
    return lastUsedId;
}




ExprTree* parseExpr(std::istream& in)
{
      /* while (! in.eof()) {
            char c; in >> c; std::cout << c;
        }*/
    ExprTree* res = parseImply(in);
    if(Token::next.type != Token::END_FILE) {
        /*while (! in.eof()) {
            std::string str;
            in >> str;
            std::cout << "|" << str << std::endl;
        }*/
        throw std::invalid_argument("titi formulae ended before the end of the file");
    }
    return res;
}

ExprTree* parseImply(std::istream& in)
{
    ExprTree* res = parseOr(in);
    if (Token::next.type == Token::IMPLY) {
        Token::nextToken(in);
        ExprTree * arg2 = parseImply(in);
        res = new Imply(res, arg2);
    }
    return res;
}

ExprTree* parseOr(std::istream& in)
{
    ExprTree* res = parseAnd(in);
    if (Token::next.type == Token::OR) {
        Token::nextToken(in);
        ExprTree * arg2 = parseOr(in);
        res = new Or(res, arg2);
    }
    return res;
}

ExprTree* parseAnd(std::istream& in)
{
    ExprTree* res = parseNot(in);
    if (Token::next.type == Token::AND) {
        Token::nextToken(in);
        ExprTree * arg2 = parseAnd(in);
        res = new And(res, arg2);
    }
    return res;
}

ExprTree* parseNot(std::istream& in)
{
    bool inverse = false;
    while (Token::next.type == Token::NOT)
    {
        Token::nextToken(in);
        inverse = !inverse;
    }
    ExprTree* res = parseVal(in);
    if (inverse) {
        res = new Not(res);
    }
    return res;
}

ExprTree* parseVal(std::istream& in)
{
    if (Token::next.type == Token::BRACE_LEFT)
    {
        Token::nextToken(in);
        ExprTree * res = parseImply(in);
        if (Token::next.type != Token::BRACE_RIGHT) {
            in >> std::noskipws;
            while(!in.eof()) {
                char c; in >> c; std::cout << c;
            }
            throw std::invalid_argument("expected right brace here");
        }
        Token::nextToken(in);
        return res;
    }
    else if(Token::next.type == Token::VAR)
    {
        std::string var = Token::next.varName();
        Token::nextToken(in);
        // cherche si la variable à déjà un numéro attribué
        // et lui attribue un numéro si non
        unsigned id;
        std::vector<std::pair<std::string,unsigned> >::const_iterator it;
        for(it = ExprTree::varNumbers.begin(); it != ExprTree::varNumbers.end(); ++it)
        {
            if (it->first == var)
                break;
        }
        if (it == ExprTree::varNumbers.end()) {
            id = ++ ExprTree::lastUsedId;
            ExprTree::varNumbers.push_back(std::pair<std::string,unsigned>(var,id));
        } else {
            id = it->second;
        }
        return new Val(id, true);
    }
    else
    {
        throw std::invalid_argument("expected open brace or variable here");
    }
}





unsigned Imply::getCNF(std::vector<clause>& cnf) const
{
    unsigned left = c1->getCNF(cnf);
    unsigned right = c2->getCNF(cnf);
    unsigned self = ++lastUsedId;
    cnf.push_back(CLAUSE3((self,false), (left,false), (right,true)));
    cnf.push_back(CLAUSE2((self,true),  (left,true)));
    cnf.push_back(CLAUSE2((self,true),  (right,false)));
    return self;
}

unsigned Or::getCNF(std::vector<clause>& cnf) const
{
    unsigned left = c1->getCNF(cnf);
    unsigned right = c2->getCNF(cnf);
    unsigned self = ++lastUsedId;
    cnf.push_back(CLAUSE3((self,false), (left,true), (right,true)));
    cnf.push_back(CLAUSE2((self,true), (left,false)));
    cnf.push_back(CLAUSE2((self,true), (right,false)));
    return self;
}

unsigned And::getCNF(std::vector<clause>& cnf) const
{
    unsigned left = c1->getCNF(cnf);
    unsigned right = c2->getCNF(cnf);
    unsigned self = ++lastUsedId;
    cnf.push_back(CLAUSE3((self,true), (left,false), (right,false)));
    cnf.push_back(CLAUSE2((self,false), (left,true)));
    cnf.push_back(CLAUSE2((self,false), (right,true)));
    return self;
}

unsigned Not::getCNF(std::vector<clause>& cnf) const
{
    unsigned fils = c1->getCNF(cnf);
    unsigned self = ++lastUsedId;
    cnf.push_back(CLAUSE2((self,false), (fils,false)));
    cnf.push_back(CLAUSE2((self,true), (fils,true)));
    return self;
}

unsigned Val::getCNF(std::vector<clause>& cnf) const
{
    cnf;
    return number;
}


