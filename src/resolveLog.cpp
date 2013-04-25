#include <fstream>
#include <queue>
#include <vector>

#include "satSolver.hh"


using namespace std;

struct compare
{
    inline bool operator () (const Literal& lit1, const Literal& lit2) const
    {
	    return lit1.var()->isOlder(lit2.var());
    }
};





void SatProblem::createConflictGraph(const Clause *conflictClause) const
{
   
    ofstream output;
    output.open ("graph.dot", ios::out);
    output << "digraph G {\n";
    output << "size =\"4,4\";\n";
    output << "error[style=filled,color=red];\n";

    // récupère le seul noeud qui doit être coloré en jaune
    Literal jaune = resolve(conflictClause).second.invert();
    output << (jaune.pos() ? ' ' : '-') << jaune.var()->varNumber << "[style=filled,color=yellow];\n";
    
    // génère tous les couples d'aretes, et colorie les sommets du pari courant en bleu
    std::vector< std::pair<Literal,Literal> > couples;

    // pour la clause contradictoire
    vector<Literal> curExamined = conflictClause->getLiterals();
    Literal conflictLiteral = curExamined.back();
    if (conflictClause->freeSize() == 1)
        conflictLiteral = conflictClause->getRemaining();
    else
        for (std::vector<Literal>::const_iterator it = curExamined.begin(); it != curExamined.end(); ++it)
            if (conflictLiteral.var()->isOlder(it->var()))
                conflictLiteral = *it;
    for (std::vector<Literal>::const_iterator it = curExamined.begin(); it != curExamined.end(); ++it)
        if (it->var() != conflictLiteral.var())
            couples.push_back( std::pair<Literal,Literal>(it->invert(), conflictLiteral) );
    output << (conflictLiteral.pos() ? ' ' : '-') << conflictLiteral.var()->varNumber << "[style=filled,color=blue];\n";
    
    // pour chaque litéral du pari courant
    for (std::vector<Variable*>::const_iterator varIt = _stackBacktrack.back(); varIt != Variable::_endDeducted; varIt++)
    {
        Literal destLit(*varIt, (*varIt)->_varState);
        if (destLit.var() != jaune.var())
            output << (destLit.pos() ? ' ' : '-') << destLit.var()->varNumber << "[style=filled,color=blue];\n";
        Clause* Exam = (*varIt)->getOriginClause();
        if (Exam == NULL)
            continue;
        vector<Literal> curExam(Exam->getLiterals());
        for (std::vector<Literal>::const_iterator it = curExam.begin(); it != curExam.end(); ++it)
            if (it->var() != *varIt)
                couples.push_back( std::pair<Literal,Literal>(it->invert(), destLit) );
    }

    
    // affiche les arêtes
    for(unsigned i = 0; i < couples.size(); i++)
    {
        output << (couples[i].first.pos() ? ' ' : '-') << couples[i].first.var()->varNumber
               << " -> "
               << (couples[i].second.pos() ? ' ' : '-') << couples[i].second.var()->varNumber
               << ";\n";
    }
    output << conflictLiteral.var()->varNumber << "->error;\n";
    output << "-" << conflictLiteral.var()->varNumber << "->error;\n";
    
    output << "}\n";
    output.close();
    
}    

   


