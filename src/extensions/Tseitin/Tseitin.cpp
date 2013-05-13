#include "Tseitin.hh"


#include <list>
#include <vector>
#include <stack>

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

static unsigned TseitinExpr(std::vector<clause>& res, std::list<Token>& tokens, unsigned& lastUsedId, std::vector<std::pair<std::string,unsigned> >& varNumbers);

static unsigned TseitinImply(std::vector<clause>& res, std::list<Token>& tokens, unsigned& lastUsedId, std::vector<std::pair<std::string,unsigned> >& varNumbers);
static unsigned TseitinAnd(std::vector<clause>& res, std::list<Token>& tokens, unsigned& lastUsedId, std::vector<std::pair<std::string,unsigned> >& varNumbers);
static unsigned TseitinOr(std::vector<clause>& res, std::list<Token>& tokens, unsigned& lastUsedId, std::vector<std::pair<std::string,unsigned> >& varNumbers);
static unsigned TseitinNot(std::vector<clause>& res, std::list<Token>& tokens, unsigned& lastUsedId, std::vector<std::pair<std::string,unsigned> >& varNumbers);
static unsigned TseitinVar(std::vector<clause>& res, std::list<Token>& tokens, unsigned& lastUsedId, std::vector<std::pair<std::string,unsigned> >& varNumbers);





unsigned ClauseTseitin(std::istream & in, std::vector<clause>& listClause, std::vector<std::pair<std::string,unsigned> >& varNumbers)
{
    // on parse les tokens :
    // tout les opérateurs binaires sont associatif, sauf l'implication qui est assiciative à droite
    // (c'est pour ça qu'on parse l'expression de la gauche vers la droite : cf TseitinImply)
    
    // récupère tous les tokens
    std::list<Token> tokList;
    do {
        tokList.push_back(Token::getToken(in));
    } while(tokList.back().type != Token::END_FILE);
    
    // parse la liste des tokens
    unsigned varId = 0;
    unsigned lastNode = TseitinExpr(listClause, tokList, varId, varNumbers);
    if(tokList.front().type != Token::END_FILE) {
        throw std::invalid_argument("forulae ended before the end of the file");
    }
    // ajoute la dernière clause : celle qui dit que la formule est vraie
    listClause.push_back(CLAUSE1((lastNode,true)));
    return varId;
}




unsigned TseitinExpr(std::vector<clause>& res, std::list<Token>& tokens, unsigned& lastUsedId, std::vector<std::pair<std::string,unsigned> >& varNumbers)
{
    return TseitinImply(res,tokens,lastUsedId, varNumbers);
}



unsigned TseitinImply(std::vector<clause>& res, std::list<Token>& tokens, unsigned& lastUsedId, std::vector<std::pair<std::string,unsigned> >& varNumbers)
{
    unsigned left = TseitinOr(res, tokens, lastUsedId, varNumbers);
    if (tokens.front().type == Token::IMPLY) {
        tokens.pop_front();
        unsigned right = TseitinImply(res,tokens,lastUsedId, varNumbers);
        unsigned self = ++lastUsedId;
        res.push_back(CLAUSE3((self,false), (left,false), (right,true)));
        res.push_back(CLAUSE2((self,true),  (left,true)));
        res.push_back(CLAUSE2((self,true),  (right,false)));
        return self;
    } else {
        return left;
    }
}

unsigned TseitinOr(std::vector<clause>& res, std::list<Token>& tokens, unsigned& lastUsedId, std::vector<std::pair<std::string,unsigned> >& varNumbers)
{
    unsigned left = TseitinAnd(res,tokens,lastUsedId, varNumbers);
    if (tokens.front().type == Token::OR) {
        tokens.pop_front();
        unsigned right = TseitinOr(res,tokens,lastUsedId, varNumbers);
        unsigned self = ++lastUsedId;
        res.push_back(CLAUSE3((self,false), (left,true), (right,true)));
        res.push_back(CLAUSE2((self,true), (left,false)));
        res.push_back(CLAUSE2((self,true), (right,false)));
        return self;
    } else {
        return left;
    }
}

unsigned TseitinAnd(std::vector<clause>& res, std::list<Token>& tokens, unsigned& lastUsedId, std::vector<std::pair<std::string,unsigned> >& varNumbers)
{
    unsigned left = TseitinNot(res,tokens,lastUsedId, varNumbers);
    if (tokens.front().type == Token::AND) {
        tokens.pop_front();
        unsigned right = TseitinAnd(res,tokens,lastUsedId,varNumbers);
        unsigned self = ++lastUsedId;
        res.push_back(CLAUSE3((self,true), (left,false), (right,false)));
        res.push_back(CLAUSE2((self,false), (left,true)));
        res.push_back(CLAUSE2((self,false), (right,true)));
        return self;
    } else {
        return left;
    }
}

unsigned TseitinNot(std::vector<clause>& res, std::list<Token>& tokens, unsigned& lastUsedId, std::vector<std::pair<std::string,unsigned> >& varNumbers)
{
    bool doNot = false;
    while (tokens.front().type == Token::NOT) {
        doNot = ! doNot;
        tokens.pop_front();
    }
    unsigned fils = TseitinVar(res,tokens,lastUsedId,varNumbers);
    if (doNot) {
        unsigned self = ++lastUsedId;
        res.push_back(CLAUSE2((self,false), (fils,false)));
        res.push_back(CLAUSE2((self,true), (fils,true)));
        return self;
    } else {
        return fils;
    }
}

unsigned TseitinVar(std::vector<clause>& res, std::list<Token>& tokens, unsigned& lastUsedId, std::vector<std::pair<std::string,unsigned> >& varNumbers)
{
    unsigned id;
    if(tokens.front().type == Token::BRACE_LEFT)
    {
        tokens.pop_front();
        id = TseitinExpr(res,tokens,lastUsedId,varNumbers);
        if(tokens.front().type != Token::BRACE_RIGHT)
            throw std::invalid_argument("expected right brace here");
        tokens.pop_front();
    }
    else if(tokens.front().type == Token::VAR)
    {
        std::string var = tokens.front().varName();
        tokens.pop_front();
        // cherche si la variable à déjà un nuéro attribué
        // et lui attribue un numéro si non
        std::vector<std::pair<std::string,unsigned> >::const_iterator it;
        for(it = varNumbers.begin(); it != varNumbers.end(); ++it)
        {
            if (it->first == var)
                break;
        }
        if (it == varNumbers.end()) {
            id = ++ lastUsedId;
            varNumbers.push_back(std::pair<std::string,unsigned>(var,id));
        } else {
            id = it->second;
        }
        return id;
    }
    else
    {
        throw std::invalid_argument("expected open brace or variable here");
    }
    return id;
}

