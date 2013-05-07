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


Token Parser::getToken()
{
    read >> std::ws;
    if (read.eof()) {
        return Token(Token::END_FILE);
    }
    char c;
    read >> c;

    // tente de lire AND
    if (c == '/')
    {
        if (read.eof() || read.peek() != '\\')
            throw std::invalid_argument("bad token : '\\' expected");
        read >> c;
        return Token(Token::AND);
    }
    // tente de lire OR
    else if (c == '\\')
    {
        if (read.eof() || read.peek() != '/')
            throw std::invalid_argument("bad token : '/' expected");
        read >> c;
        return Token(Token::OR);
    }
    // tente de lire NOT
    else if (c == '~')
    {
        return Token(Token::NOT);
    }
    // tente de lire IMPLY
    else if (c == '=')
    {
        if (read.eof() || read.peek() != '>')
            throw std::invalid_argument("bad token : '>' expected");
        read >> c;
        return Token(Token::IMPLY);
    }
    // tente de lire VAR
    else if (isLetter(c))
    {
        std::string var(1,c);
        while (!read.eof() && isIdentLetter(read.peek()))
        {
            read >> c;
            var += c;
        }
        // cherche si le nom de la variable à déjà été rencontré
        unsigned num;
        const std::map<std::string,unsigned>::const_iterator numIt = _varsNumbers.find(var);
        if (numIt != _varsNumbers.end())
        {
            num = numIt->second;
        }
        else
        {
            num = _varsNumbers.size() + 1;
            _varsNumbers.insert(std::pair<std::string,unsigned>(var,num));
        }
        return Token(num);
    }
    // tente de lire BRACE_LEFT
    else if (c == '(')
    {
        return Token(Token::BRACE_LEFT);
    }
    // tente de lire BRACE_RIGTH
    else if (c == ')')
    {
        return Token(Token::BRACE_RIGHT);
    }
    // sinon : erreur de format d'entrée
    else
    {
        throw std::invalid_argument("unrecognized token");
    }
}

