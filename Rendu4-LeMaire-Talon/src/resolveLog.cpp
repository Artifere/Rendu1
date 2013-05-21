#include <fstream>
#include <vector>
#include <utility>

#include "satSolver.hh"
#include "Debug.hh"




void SatProblem::createConflictGraph(Variable *conflictVar) const
{

    std::ofstream output;
    output.open ("graph.dot", std::ios::out);
    output << "digraph G {\n";
    output << "size =\"4,4\";\n";
    output << "error[style=filled,color=red];\n";

    // récupère le seul noeud qui doit être coloré en jaune
    Literal jaune = resolve(conflictVar).second.invert();
    output << (jaune.pos() ? ' ' : '-') << jaune.var()->varNumber << "[style=filled,color=yellow];\n";
    
    // liste des arêtes du graphe
    std::vector< std::pair<Literal,Literal> > couples;

    // colorie toutes les variables du pari courant en bleu
    // et affiche les arêtes qui ont permi de la déduire
    for(std::vector<Variable*>::const_iterator varIt = _stackBacktrack.back(); varIt != Variable::_endDeducted; varIt++)
    {
        Literal destLit(*varIt, (*varIt)->_varState);
        // colorie en bleu ssi ce n'est pas le littéral jaune
        if (destLit.var() != jaune.var())
            output << (destLit.pos() ? ' ' : '-') << destLit.var()->varNumber << "[style=filled,color=blue];\n";
        // et ajoute les arêtes provennant les litéraux qui ont permis de le déduire
        Clause* origin = destLit.var()->getOriginClause(destLit.pos());
        // il n'y a pas d'arete si la clause était de taille 1
        if (origin == NULL)
            continue;
        for (std::vector<Literal>::const_iterator it = origin->getLiterals().begin(); it != origin->getLiterals().end(); ++it)
        {
            if (it->var() != *varIt)
                couples.push_back( std::pair<Literal,Literal>(it->invert(), destLit) );
        }
    }

    // colorie en bleu le contraire de la variable conflictuelle (pas pris en compte dans la boucle précédente)
    // et ajoute les arêtes provennant les litéraux qui ont permis de la déduire
    Literal destLit(conflictVar, !conflictVar->_varState);
    output << (conflictVar->_varState ? '-' : ' ') << conflictVar->varNumber << "[style=filled,color=blue];\n";
    Clause* origin = conflictVar->getOriginClause(!conflictVar->_varState);
    if (origin != NULL) // normalement c'est toujour le cas ...
    {
        for (std::vector<Literal>::const_iterator it = origin->getLiterals().begin(); it != origin->getLiterals().end(); ++it)
        {
            if (it->var() != conflictVar)
                couples.push_back( std::pair<Literal,Literal>(it->invert(), destLit) );
        }
    }
    
    // affiche les arêtes
    for(unsigned i = 0; i < couples.size(); i++)
    {
        output << (couples[i].first.pos() ? ' ' : '-') << couples[i].first.var()->varNumber
               << " -> "
               << (couples[i].second.pos() ? ' ' : '-') << couples[i].second.var()->varNumber
               << ";\n";
    }
    output << conflictVar->varNumber << "->error;\n";
    output << '-' << conflictVar->varNumber << "->error;\n";
    
    output << "}\n";
    output.close();
}    

   


