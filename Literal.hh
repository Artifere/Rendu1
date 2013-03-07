#ifndef LITERAL_HH
#define LITERAL_HH


typedef enum varState
{
    TRUE, FALSE, FREE
} varState;




class Literal
{
protected:
    unsigned int _var;
    bool _pos;

public:
    inline Literal(unsigned int var, bool pos) : _var(var), _pos(pos) { };
    inline Literal(const Literal& lit) : _var(lit._var), _pos(lit._pos) { };


    inline unsigned int var(void) const
    {
        return _var;
    };

    inline bool pos(void) const
    {
        return _pos;
    };


    inline void invert()
    {
        _pos = !_pos;
    };

    inline bool operator<(const Literal& lit) const
    {
        return _var < lit._var || (_var == lit._var && !_pos && lit._pos);
    };
};




#endif//LITERAL_HH
