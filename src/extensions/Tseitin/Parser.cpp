#include "Parser.hh"
#include <iostream>

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





std::vector<bool> readAssignation(std::istream& read) 
{
    //std::cout << "nombre de variables : " << nbrVars << std::endl;
    std::vector<bool> assign(1);
    std::string str;
    char c;
    
    // skip initials comments
    while ((read >> std::ws) && (! read.eof()) && read.peek() == 'c')
    {
        getline(read, str); // skip the line
        //std::cout << "skip the line " << str << std::endl;
    }
    
    read >> std::ws >> c >> str;
    if ((c != 's') || (str != "SATISFIABLE")) {
        //std::cout << "quit : unkown out " << c << '|' << str << std::endl;
        assign.clear();
        return assign;
    }

    while (! read.eof())
    {
        read >> std::ws >> c >> std::ws;
        while (c == 'c') {
            getline(read,str);
            read >> std::ws >> c;
            //std::cout << "skip the line " << str << std::endl;
        }
        if (c != 'v') {
            assign.clear();
            return assign;
        }
        int var = 0;
        //if(! (read >> var)) {
        //    std::cout << "ecture de int impossible. prochain caractère : " <<  (char)read.peek() < std::endl;
        //}
        read >> var >> std::ws;
        //std::cout << "lecture variable : " << var << std::endl;
        literal lit = (var < 0) ? literal(-var, false) : literal(var, true);
        if (assign.size() <= lit.first)
            assign.resize(lit.first+1);
        assign[lit.first] = lit.second;
    }
    return assign;
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
            throw std::invalid_argument("bad token : do you mean '\\' ?");
        _read >> c;
        _type = AND;
    }
    // tente de lire OR
    else if (c == '\\')
    {
        if (_read.eof() || _read.peek() != '/')
            throw std::invalid_argument("bad token : do yu mean '/' ?");
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
            throw std::invalid_argument("bad token : do you mean '>' ?");
        _read >> c;
        _type = IMPLY;
    }
    // tente de lire EQUIV
    else if (c == '<')
    {
        if (_read.eof() || _read.peek() != '=')
            throw std::invalid_argument("bad token : do you mean '<=>' ?");
        _read >> c;
        if (_read.eof() || _read.peek() != '>')
            throw std::invalid_argument("bad token : do you mean '<=>' ?");
        _read >> c;
        _type = EQUIV;
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
    ExprTree* res = parseEquiv(false);
    if(tok.type() != Token::END_FILE) {
        std::cout<< std::endl << std::endl << tok.type() << std::endl<< std::endl;
        while (!std::cin.eof()) {
            std::string str;
            std::cin >> str;
            std::cout << str << std::endl;
        }
        throw std::invalid_argument("formulae ended before the end of the file");
    }
    return res;
}

ExprTree* ParserExprTree::parseEquiv(bool invert)
{
    //std::cout << "parse equiv " << invert  << std::endl;
    ExprTree* res = parseImply(invert);
    // on remplace une équivalence par arg1&arg2 | non(arg1)&non(arg2)
    // ie par arg1|non(arg2) && non(arg1)|arg2
    if (tok.type() == Token::EQUIV) {
        //std::cout << "milieu equiv" << std::endl;
        tok.readNext();
        ExprTree * arg1 = res;
        ExprTree * arg2 = parseEquiv(invert);
        ExprTree * n_arg1 = arg1->inversion();
        ExprTree * n_arg2 = arg2->inversion();
        //res = new And(new Or(n_arg1, arg2), new Or(arg1, n_arg2));
        if (invert)
            res = new And(new Or(arg1, arg2), new And(n_arg1, n_arg2));
        else
            res = new Or(new And(arg1, arg2), new And(n_arg1, n_arg2));
    }
    //std::cout << "fin equiv : " << res << std::endl;
    return res;
}

ExprTree* ParserExprTree::parseImply(bool invert)
{
    //std::cout << "parse imply " << invert  << std::endl;
    ExprTree* res = parseOr(invert);
   // on remplace une implication par non(arg1)|arg2
    if (tok.type() == Token::IMPLY) {
        //std::cout << "milieu imply" << std::endl;
        tok.readNext();
        ExprTree * arg1 = res;
        ExprTree * arg2 = parseImply(invert);
        ExprTree * n_arg1 = arg1->inversion();
        if (invert)
            res = new And(n_arg1, arg2);
        else
            res = new Or(n_arg1, arg2);
        delete arg1; // on n'utilise plus arg1
    }
    //std::cout << "fin imply : " << res << std::endl;
    return res;
}

ExprTree* ParserExprTree::parseOr(bool invert)
{
    //std::cout << "parse or " << invert  << std::endl;
    ExprTree* res = parseAnd(invert);
    if (tok.type() == Token::OR) {
        //std::cout << "milieu or" << std::endl;
        tok.readNext();
        ExprTree * arg2 = parseOr(invert);
        if (invert)
            res = new And(res, arg2);
        else
            res = new Or(res, arg2);
    }
    //std::cout << "fin or : " << res << std::endl;
    return res;
}

ExprTree* ParserExprTree::parseAnd(bool invert)
{
    //std::cout << "parse and " << invert << std::endl;
    ExprTree* res = parseNot(invert);
    if (tok.type() == Token::AND) {
        //std::cout << "milieu and " << std::endl;
        tok.readNext();
        ExprTree * arg2 = parseAnd(invert);
        if (invert)
            res = new Or(res, arg2);
        else
            res = new And(res, arg2);
    }
    //std::cout << "fin and : " << res << std::endl;
    return res;
}

ExprTree* ParserExprTree::parseNot(bool invert)
{
    //std::cout << "parse not";
    while (tok.type() == Token::NOT)
    {
        //std::cout << " "<<invert<<" ";
        tok.readNext();
        invert = !invert;
    }
    //std::cout << std::endl;
    return parseVal(invert);
}

ExprTree* ParserExprTree::parseVal(bool invert)
{
    if (tok.type() == Token::BRACE_LEFT)
    {
        //std::cout << "parse braces  " << invert << std::endl;
        tok.readNext();
        ExprTree * res = parseEquiv(invert);
        if (tok.type() != Token::BRACE_RIGHT) {
            throw std::invalid_argument("expected right brace here");
        }
        //std::cout << "fin braces : " << res << std::endl;
        tok.readNext();
        return res;
    }
    else if(tok.type() == Token::VAR)
    {
        //std::cout << "parse var " << invert  << std::endl;
        std::string var = tok.varName();
        tok.readNext();
        return new Val(var, !invert);
    }
    else
    {
        throw std::invalid_argument("expected open brace or variable here");
    }
}




