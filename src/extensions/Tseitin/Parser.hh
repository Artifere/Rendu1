#ifndef TRANSFORM_PARSER_HH
#define TRANSFORM_PARSER_HH


#include <istream>
#include <stdexcept>
#include <string>
#include <cassert>
#include <map>
#include "ExprTree.hh"


class Token
{
public:
    typedef enum
    {
        END_FILE, AND, OR, NOT, IMPLY, EQUIV, VAR, BRACE_LEFT, BRACE_RIGHT
    } token_type;

protected:
    std::string _varName; // only usefull if type == VAR
    std::istream& _read;
    token_type _type;

public:
    
    inline Token(std::istream& read) : _varName(), _read(read), _type(END_FILE) { readNext(); };
    
    inline token_type type() const { return _type; }
    inline std::string varName(void) const { assert(_type == VAR); return _varName; }
    
    void readNext();
};



class ParserExprTree
{
protected:
    Token tok;
    
    ExprTree* parseEquiv(bool invert);  // invert indique si on doit à inverser ce qu'on lit ou non
    ExprTree* parseImply(bool invert);  // le but étant de parser les ~, mais de ne les prendre en compte que au niveau des litéraux
    ExprTree* parseOr(bool invert);     // comme ça, on n'a pas besoin d'une transformation de Tseitin pour l'opération NOT
    ExprTree* parseAnd(bool invert);
    ExprTree* parseNot(bool invert);
    ExprTree* parseVal(bool invert);

public:
    inline ParserExprTree(std::istream& in) : tok(in) { }
    ExprTree* parseExpr();
};



#endif//TRANSFORM_PARSER_HH
