#ifndef DEBUG_HH
#define DEBUG_HH

#include <ostream>
#include <iomanip>
#include <vector>



#define DEBUG(n) if (VERBOSE < n) { } else std::cout << "c "



/*
// autre essai : fonctionne, mais plus lent que juste la macro
inline std::ostream& debug(const unsigned verbose, std::ostream& realOut = std::cout) {
    static std::ostream nullOut(0);
    static const unsigned verboseLevel = VERBOSE;
    
    std::ostream& out = (verbose <= verboseLevel) ? realOut : nullOut;
    out << "c ";
    unsigned stackSize = Variable::_endAssigned - Variable::_vars.begin();
    while (stackSize --)
        out << "| ";
    return out;
}
*/

class Variable;
std::ostream& operator<< (std::ostream& out, const Variable& var);


class WatchedClause;
std::ostream& operator<< (std::ostream& out, const WatchedClause* c);

class SmartClause;
std::ostream& operator<< (std::ostream& out, const SmartClause* c);


class Literal;
std::ostream& operator<< (std::ostream& out, const Literal& v);

class SatProblem;
std::ostream& operator<< (std::ostream& out, const SatProblem& sp);


std::ostream& operator<< (std::ostream& out, const std::vector<Literal>& v);




#if 0

/* opérateur de flux générique */

template<typename T>
inline Debug& operator<<(Debug& debug, const T& a)
{
    if (debug.currentV >= debug.verbose)
      debug.outStream << a;
    return debug;
}

/* opérateur endl de fin de ligne */

inline Debug& std::endl (Debug& debug)
{
    std::endl (debug.outStream);
}





/* spécialisation pour ouvrir un message */

struct Verbose // pour changer le niveau de verbose
{
    const unsigned verbose;
    inline Verbose(const unsigned v) : verbose(v) { };
};

template <>
inline Debug& operator<< <Verbose>(Debug& debug, const Verbose& v)
{
    debug.currentV = v.verbose;
    // ouverture du message de debug
    debug << "c ";
    unsigned stackSize = Variable::_endAssigned - Variable::_vars.begin();
    while (stackSize --)
        debug << "| ";
    return debug;
}





/* spécialisation pour les types du SatSolver */


/* Literal */

template <>
inline Debug& operator<< <Literal>(Debug& debug, const Literal& v)
{
    return debug << v.vuar() << '.' << v.pos();
}


/* Variable et Variable* */

template<>
inline Debug& operator<<(Debug& debug, const Variable& var)
{
    if (var.isFree())
        debug << "?";
    else if (var._varState)
        debug << "+";
    else
        debug << "-";
    return debug << std::setw(2) << std::setfill('_') << var.varNumber << std::setw(1);
    #if 0 && verbose > 10
    std::cout << '(';
    for(unsigned j = 0; j < _litTrue.size(); j++)
      std::cout << _litTrue[j]->clauseNumber << ".";
    std::cout << ", ";
    for(unsigned j = 0; j < _litFalse.size(); j++)
      std::cout << _litFalse[j]->clauseNumber << ".";
    std::cout << ')';
    #endif
}


#endif




#endif//DEBUG_HH
