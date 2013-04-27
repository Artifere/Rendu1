#include "Debug.hh"
#include <iostream>


#include "Literal.hh"
#include "Clause.hh"
#include "Variable.hh"
#include "satSolver.hh"


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



std::ostream& operator<< (std::ostream& out, const Variable& var)
{
    if (var.isFree())
        out << "?";
    else if (var._varState)
        out << "+";
    else
        out << "-";
    out << std::setfill(' ') << std::setw(3) << std::left << var.varNumber << std::setw(0);
    
    /*
    signed v = (var._varState) ? (signed)var.varNumber : -(signed)var.varNumber;
    out << std::setfill(' ') << std::setw(4) << std::right << std::showpos << v << std::setw(0) << std::noshowpos;
    */
    return out;
}




std::ostream& operator<< (std::ostream& out, const SatProblem& sp)
{
    std::vector<Variable*>::const_iterator it;
    for (it = Variable::_vars.begin(); it < Variable::_endAssigned; ++ it)
    {
        if (std::find (sp._stackBacktrack.begin(), sp._stackBacktrack.end(), it) != sp._stackBacktrack.end())
            out << "\033[1;31m";
        else
            out << "\033[1;34m";
        out << *(*it);
    }
    out << "\033[0m";
    for ( ; it < Variable::_endDeducted; ++ it)
        out << *(*it);
    return out;
}



std::ostream& operator<< (std::ostream& out, const Literal& v)
{
    if (v.var() == NULL)
        out << "(nil)";
    else
        out << v.var()->varNumber << '.' << v.pos();
    return out;
}



std::ostream& operator<< (std::ostream& out, const Clause* c)
{
    if (c == NULL)
        out << "de taille 1";
    else
        out << c->clauseNumber;
    return out;
}



std::ostream& operator<< (std::ostream& out, const std::vector<Literal>& v)
{
    if (v.empty())
        out << "(nil)";
    else
    {
        std::vector<Literal>::const_iterator it = v.begin();
        out << *(it ++);
        while (it != v.end())
            out << ", " << *(it ++);
    }
    return out;
}



