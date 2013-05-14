#include "Parser.hh"



static inline bool
isLetter(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static inline bool
isIdentLetter(char c)
{
    return isLetter(c) || c == '_' || (c >= '0' && c <= '9');
}


void Token::readNext()
{
    _read >> std::ws;
    if (_read.eof()) {
        _type = END_FILE;
        return;
    }
    char c;
    _read >> c;

    // tente de lire AND
    if (c == '/')
    {
        if (_read.eof() || _read.peek() != '\\')
            throw std::invalid_argument("bad token : '\\' expected");
        _read >> c;
        _type = AND;
    }
    // tente de lire OR
    else if (c == '\\')
    {
        if (_read.eof() || _read.peek() != '/')
            throw std::invalid_argument("bad token : '/' expected");
        _read >> c;
        _type = OR;
    }
    // tente de lire NOT
    else if (c == '~')
    {
        _type = NOT;
    }
    // tente de lire IMPLY
    else if (c == '=')
    {
        if (_read.eof() || _read.peek() != '>')
            throw std::invalid_argument("bad token : '>' expected");
        _read >> c;
        _type = IMPLY;
    }
    // tente de lire VAR
    else if (isLetter(c))
    {
        std::string var(1,c);
        while (!_read.eof() && isIdentLetter(_read.peek()))
        {
            _read >> c;
            var += c;
        }
        _type = VAR;
        _varName = var;
    }
    // tente de lire BRACE_LEFT
    else if (c == '(')
    {
        _type = BRACE_LEFT;
    }
    // tente de lire BRACE_RIGTH
    else if (c == ')')
    {
        _type = BRACE_RIGHT;
    }
    // sinon : erreur de format d'entrée
    else
    {
        throw std::invalid_argument("unrecognized token");
    }
}








ExprTree* ParserExprTree::parseExpr()
{
    ExprTree* res = parseImply(false);
    if(tok.type() != Token::END_FILE) {
        throw std::invalid_argument("titi formulae ended before the end of the file");
    }
    return res;
}

ExprTree* ParserExprTree::parseImply(bool invert)
{
    ExprTree* res = parseOr(invert);
    // on remplace une implication par non(arg1)|arg2
    if (tok.type() == Token::IMPLY) {
        tok.readNext();
        ExprTree * arg2 = parseImply(invert);
        // on a déjà parsé arg1. il faut l'inverser maintenant
        ExprTree * arg1 = res->inversion();
        delete res;
        res = new Or(arg1, arg2);
    }
    return res;
}

ExprTree* ParserExprTree::parseOr(bool invert)
{
    ExprTree* res = parseAnd(invert);
    if (tok.type() == Token::OR) {
        tok.readNext();
        ExprTree * arg2 = parseOr(invert);
        if (invert)
            res = new And(res, arg2);
        else
            res = new Or(res, arg2);
    }
    return res;
}

ExprTree* ParserExprTree::parseAnd(bool invert)
{
    ExprTree* res = parseNot(invert);
    if (tok.type() == Token::AND) {
        tok.readNext();
        ExprTree * arg2 = parseAnd(invert);
        if (invert)
            res = new Or(res, arg2);
        else
            res = new And(res, arg2);
    }
    return res;
}

ExprTree* ParserExprTree::parseNot(bool invert)
{
    while (tok.type() == Token::NOT)
    {
        tok.readNext();
        invert = !invert;
    }
    return parseVal(invert);
}

ExprTree* ParserExprTree::parseVal(bool invert)
{
    if (tok.type() == Token::BRACE_LEFT)
    {
        tok.readNext();
        ExprTree * res = parseImply(invert);
        if (tok.type() != Token::BRACE_RIGHT) {
            throw std::invalid_argument("expected right brace here");
        }
        tok.readNext();
        return res;
    }
    else if(tok.type() == Token::VAR)
    {
        std::string var = tok.varName();
        tok.readNext();
        return new Val(var, !invert);
    }
    else
    {
        throw std::invalid_argument("expected open brace or variable here");
    }
}




