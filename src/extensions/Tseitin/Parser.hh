#ifndef TRANSFORM_PARSER_HH
#define TRANSFORM_PARSER_HH


#include <istream>
#include <stdexcept>
#include <string>
#include <cassert>
#include <map>


class Token
{
public:
    typedef enum
    {
        END_FILE, AND, OR, NOT, IMPLY, VAR, BRACE_LEFT, BRACE_RIGHT
    } token_type;

protected:
    // only usefull if type == VAR
    const unsigned _varNumber;

public:
    const token_type type;
    
    inline Token(token_type t) : _varNumber(0U), type(t) { assert(t != VAR); };
    inline Token(unsigned var) : _varNumber(var), type(VAR) { };
    
    unsigned varNumber(void) const { assert(type == VAR); return _varNumber; }
};



// objet qui parser (tokenise) l'entrée,
// et transforme les noms de variables en numéro

class Parser
{
protected:
    std::istream& read;
    std::map<std::string, unsigned> _varsNumbers;
    
    //Token next;
    
public:
    Token getToken();

    inline const std::map<std::string, unsigned>& vars() const { return _varsNumbers; };

    inline Parser(std::istream& in) : read(in) { };
    
    /*inline Token nextToken() {
        if(next.type == END_FILE) {
            return next;
        } else {
            Token ret = next;
            next = getToken();
            return ret;
        }
    }*/
};



#endif//TRANSFORM_PARSER_HH
