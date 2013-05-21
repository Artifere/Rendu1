#include <iostream>
#include "Debug.hh"


#include "Literal.hh"
#include "Clause.hh"
#include "Variable.hh"
#include "satSolver.hh"



/** 
    Fichier contenant les fonctions d'affichage des différentes structures, pour
    le debug
**/



/* Affiche l'état d'une variable */
std::ostream& operator<< (std::ostream& out, const Variable& var)
{
    if (var.isOlderIter(Variable::_endDeducted))
        out << "?";
    else if (var._varState)
        out << "+";
    else
        out << "-";
    out << std::setfill(' ') << std::setw(3) << std::left << var.varNumber << std::setw(0);
   
    return out;
}


/* Affiche l'état du backtrack dans le solveur sat */
std::ostream& operator<< (std::ostream& out, const SatProblem& sp)
{
    std::vector<Variable*>::const_iterator it;
    for (it = Variable::_vars.begin(); it < Variable::_endAssigned; ++it)
    {
        if (std::find(sp._stackBacktrack.begin(), sp._stackBacktrack.end(), it) != sp._stackBacktrack.end())
            out << "\033[1;31m";
        else
            out << "\033[1;34m";
        out << *(*it);
    }
    out << "\033[0m";
    for ( ; it < Variable::_endDeducted; ++it)
        out << *(*it);
    return out;
}



/* Affiche un litéral : variable.valeur (valeur = 0 si la variable est niée, 1 sinon) */
std::ostream& operator<< (std::ostream& out, const Literal& v)
{
    if (v.var() == NULL)
        out << "(nil)";
    else
        out << v.var()->varNumber << '.' << v.pos();
    return out;
}



/* Affiche les litéraux d'une clause */
std::ostream& operator<< (std::ostream& out, const Clause* c)
{
    if (c == NULL)
        out << "de taille 1";
    else
    {
        out << c->clauseNumber;
        #if VERBOSE > 10
        out << "(" << c->getLiterals() << ")";
        #endif
    }
    return out;
}



/* Affiche les litéraux d'une clause, qu'on passe en paramètres */
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



