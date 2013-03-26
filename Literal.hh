#ifndef LITERAL_HH
#define LITERAL_HH

#include "Variable.hh"



class Literal
{
protected:
    Variable* _var;
    bool _pos;

public:
    inline Literal() : _var(NULL), _pos(true) { };
    inline Literal(Variable* var, bool pos) : _var(var), _pos(pos) { };
    inline Literal(const Literal& lit) : _var(lit._var), _pos(lit._pos) { };


    inline Variable* var(void) const
    {
        return _var;
    };

    inline bool pos(void) const
    {
        return _pos;
    };


    inline Literal invert() const
    {
        return Literal(_var, !_pos);
    };

    inline bool operator<(const Literal& lit) const
    {
        return _var < lit._var || (_var == lit._var && !_pos && lit._pos);
    };

    // Renvoie si le litéral est vrai ou non étant donné l'assignation en cours
    inline bool isTrue(void) const
    {
        const varState v = _var->_varState;
        return (v == TRUE && _pos) || (v == FALSE && !_pos);
    };

};




#endif // LITERAL_HH defined
