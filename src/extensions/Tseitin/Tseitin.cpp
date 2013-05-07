#include "ExprTree.hh"


#include <list>
#include <vector>
#include <stack>

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

#define CLAUSE2(a,b) doClause(literal a, literal b)
#define CLAUSE3(a,b,c) doClause(literal a, literal b, literal c)

static unsigned TseitinExpr(std::vector<clause>& res, std::list<Token>& tokens, unsigned& lastUsedId);

static unsigned TseitinImply(std::vector<clause>& res, std::list<Token>& tokens, unsigned& lastUsedId);
static unsigned TseitinAnd(std::vector<clause>& res, std::list<Token>& tokens, unsigned& lastUsedId);
static unsigned TseitinOr(std::vector<clause>& res, std::list<Token>& tokens, unsigned& lastUsedId);
static unsigned TseitinNot(std::vector<clause>& res, std::list<Token>& tokens, unsigned& lastUsedId);
static unsigned TseitinVar(std::vector<clause>& res, std::list<Token>& tokens, unsigned& lastUsedId);



unsigned ClauseTseitin(Parser & parser, std::vector<clause>& listClause)
{
    // on parse les tokens :
    // tout les opérateurs binaires sont associatif, sauf l'implication qui est assiciative à droite
    // (c'est pour ça qu'on parse l'expression de la gauche vers la droite : cf TseitinImply)
    
    // récupère tous les tokens
    std::list<Token> tokList;
    do {
        tokList.push_back(parser.getToken());
    } while(tokList.back().type != Token::END_FILE);
    
    // récupère le plus grand numéro de variable
    unsigned varId = 0;
    for(std::list<Token>::const_iterator it = tokList.begin(); it != tokList.end(); ++it) {
        if(it->type == Token::VAR && varId < it->varNumber()) {
            varId = it->varNumber();
        }
    }
    
    // parse vraiment la liste des tokens
    TseitinExpr(listClause, tokList, varId);
    if(tokList.front().type != Token::END_FILE) {
        throw std::invalid_argument("forulae ended before the end of the file");
    }
    return varId;
}




unsigned TseitinExpr(std::vector<clause>& res, std::list<Token>& tokens, unsigned& lastUsedId)
{
    return TseitinImply(res,tokens,lastUsedId);
}



unsigned TseitinImply(std::vector<clause>& res, std::list<Token>& tokens, unsigned& lastUsedId)
{
    unsigned left = TseitinOr(res, tokens, lastUsedId);
    if (tokens.front().type == Token::IMPLY) {
        tokens.pop_front();
        unsigned right = TseitinImply(res,tokens,lastUsedId);
        unsigned self = ++lastUsedId;
        res.push_back(CLAUSE3((self,false), (left,false), (right,true)));
        res.push_back(CLAUSE2((self,true),  (left,true)));
        res.push_back(CLAUSE2((self,true),  (right,false)));
        return self;
    } else {
        return left;
    }
}

unsigned TseitinOr(std::vector<clause>& res, std::list<Token>& tokens, unsigned& lastUsedId)
{
    unsigned left = TseitinAnd(res,tokens,lastUsedId);
    if (tokens.front().type == Token::OR) {
        tokens.pop_front();
        unsigned right = TseitinOr(res,tokens,lastUsedId);
        unsigned self = ++lastUsedId;
        res.push_back(CLAUSE3((self,false), (left,true), (right,true)));
        res.push_back(CLAUSE2((self,true), (left,false)));
        res.push_back(CLAUSE2((self,true), (right,false)));
        return self;
    } else {
        return left;
    }
}

unsigned TseitinAnd(std::vector<clause>& res, std::list<Token>& tokens, unsigned& lastUsedId)
{
    unsigned left = TseitinNot(res,tokens,lastUsedId);
    if (tokens.front().type == Token::AND) {
        tokens.pop_front();
        unsigned right = TseitinAnd(res,tokens,lastUsedId);
        unsigned self = ++lastUsedId;
        res.push_back(CLAUSE3((self,true), (left,false), (right,false)));
        res.push_back(CLAUSE2((self,false), (left,true)));
        res.push_back(CLAUSE2((self,false), (right,true)));
        return self;
    } else {
        return left;
    }
}

unsigned TseitinNot(std::vector<clause>& res, std::list<Token>& tokens, unsigned& lastUsedId)
{
    bool doNot = false;
    while (tokens.front().type == Token::NOT) {
        doNot = ! doNot;
        tokens.pop_front();
    }
    unsigned fils = TseitinVar(res,tokens,lastUsedId);
    if (doNot) {
        unsigned self = ++lastUsedId;
        res.push_back(CLAUSE2((self,false), (fils,false)));
        res.push_back(CLAUSE2((self,true), (fils,true)));
        return self;
    } else {
        return fils;
    }
}

unsigned TseitinVar(std::vector<clause>& res, std::list<Token>& tokens, unsigned& lastUsedId)
{
    unsigned id;
    if(tokens.front().type == Token::BRACE_LEFT) {
        tokens.pop_front();
        id = TseitinExpr(res,tokens,lastUsedId);
        if(tokens.front().type != Token::BRACE_RIGHT)
            throw std::invalid_argument("expected right brace here");
        tokens.pop_front();
    } else if(tokens.front().type == Token::VAR) {
        id = tokens.front().varNumber();
        tokens.pop_front();
    } else {
        throw std::invalid_argument("expected open brace or variable here");
    }
    return id;
}

