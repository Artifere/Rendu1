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
    std::string _varName;

public:
    token_type type;
    
    inline Token(token_type t) : _varName(), type(t) { assert(t != VAR); };
    inline Token(const std::string& var) : _varName(var), type(VAR) { };
    
    std::string varName(void) const { assert(type == VAR); return _varName; }
    
    static Token getToken(std::istream& read);
    static Token next;
    static inline void nextToken(std::istream& read)
    {
        next = getToken(read);
    }
};


#endif//TRANSFORM_PARSER_HH
