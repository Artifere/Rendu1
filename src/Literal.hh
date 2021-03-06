#ifndef LITERAL_HH
#define LITERAL_HH

#include "Variable.hh"
#include <iostream>



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
        return _var < lit._var || (_var == lit._var && _pos && !lit._pos);
    }

    inline bool operator==(const Literal& lit) const
    {
        return (_var == lit._var) && (_pos == lit._pos);
    }

    /* Renvoie si le litéral est vrai ou non étant donné l'assignation en cours */
    inline bool isTrue(void) const
    {
        return (! isFree()) && (_pos == _var->_varState);
    }
    /* Renvoie si le litéral concerne une variable non encore assignée */
    inline bool isFree(void) const
    {
        return _var->isOlderIter(Variable::_endDeducted);
    }

    friend std::ostream& operator<< (std::ostream& out, const Literal&);
};



#endif // LITERAL_HH defined
