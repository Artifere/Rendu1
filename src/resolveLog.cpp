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





void SatProblem::createConflictGraph(const Clause *conflictClause)
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
    
}    


    // affiche chaque arête
   
        
#if 0
void SatProblem::createConflictGraph(const Clause *conflictClause)
{
	static int truc = 0;


    vector<Literal> curExamined = conflictClause->getLiterals();
	vector<bool> seen(this->_nbrVars+1, false);
	priority_queue<Literal, std::vector<Literal>, compare> toDraw;
    
    Literal conflictLiteral = curExamined.back();
    if (conflictClause->freeSize() == 1)
        conflictLiteral = conflictClause->getRemaining();
    else
    {
        for (std::vector<Literal>::const_iterator it = curExamined.begin()+1; it != curExamined.end(); ++it)
        {
            if (conflictLiteral.var()->isOlder(it->var()))
                conflictLiteral = *it;
        }
    }

	ofstream output;
	output.open ("graph.dot", ios::out);
	output << "digraph G {\n";
	output << "size =\"4,4\";\n";
	
	
	output << conflictLiteral.var()->varNumber << "[color=blue];\n";
	output << "-" << conflictLiteral.var()->varNumber << "[color=blue];\n";
	output << "error[color=red];\n";
	output << conflictLiteral.var()->varNumber << "->error;\n";
	output << "-" << conflictLiteral.var()->varNumber << "->error;\n";
   
   
	

	const  vector<Literal> originConflictLiterals = conflictLiteral.var()->getOriginClause()->getLiterals();
    for (std::vector<Literal>::const_iterator it = originConflictLiterals.begin(); it != originConflictLiterals.end(); ++it)
    {
        if (true)//!seen[it->var()->varNumber])
		{
			seen[it->var()->varNumber] = true;
	        toDraw.push(*it);
	    }
    }
	
	for (std::vector<Literal>::const_iterator it = curExamined.begin(); it != curExamined.end(); ++it)
    {
		if (!seen[it->var()->varNumber])
		{
			seen[it->var()->varNumber] = true;
			toDraw.push(*it);
		}
		
		if (it->var() != conflictLiteral.var())
		{
			const Literal originLiteral = *it;//->getOriginClause()->getRemaining();
							
			output << (originLiteral.pos()?"-":"") << originLiteral.var()->varNumber;
			if (originLiteral.var()->isFromCurBet(_stackBacktrack.back()))
				output << "[color=blue];\n";

			output << (originLiteral.pos()?"-":"") << originLiteral.var()->varNumber;
			output << "->" << (conflictLiteral.pos()?"-":"") << conflictLiteral.var()->varNumber;
			output << ";\n";
		}
    }
	
	

	
    while (toDraw.size() != 0)//toDraw.top().var() != *_stackBacktrack.back())
    {
        const Literal cur = toDraw.top();
        toDraw.pop();
		if (cur.var() == NULL)
			continue;
		seen[cur.var()->varNumber] = true;
		if (cur.var()->getOriginClause() == NULL)
			continue;
        std::cout << "DEBUG LOG : var = " << cur.var()->varNumber << ", clause = " << cur.var()->getOriginClause()->clauseNumber  << std::endl;
        
        curExamined = cur.var()->getOriginClause()->getLiterals();
		cout << "taille du tas = " << toDraw.size() << std::endl;
        
		
		
		cout << toDraw.size() << endl;
		output << (cur.pos()?"-":"") << cur.var()->varNumber;
        
        if (cur.var()->isFromCurBet(_stackBacktrack.back()))
		{
			if (toDraw.size() == 1)
				output << "[color=yellow];\n";
			else
				output << "[color=blue];\n";
		}
		
        for (std::vector<Literal>::const_iterator it = curExamined.begin(); it != curExamined.end(); ++it)
        {
            if (it->var() != cur.var())
            {
				const Literal originLiteral = *it;//->getOriginClause()->getRemaining();
							
				output << (originLiteral.pos()?"-":"") << originLiteral.var()->varNumber;
				if (originLiteral.var()->isFromCurBet(_stackBacktrack.back()))
					output << "[color=blue];\n";

				output << (originLiteral.pos()?"-":"") << originLiteral.var()->varNumber;
				output << "->" << (cur.pos()?"-":"") << cur.var()->varNumber;
				output << ";\n";
            }
            if ( !seen[it->var()->varNumber])
            {
                seen[it->var()->varNumber] = true;
                toDraw.push(*it);
            }
        }
		//if (toDraw.size() < 1)
			//break;
		
		/*const Literal originLiteral = cur.var()->getOriginClause()->getRemaining();
		output << (originLiteral.pos()?"":"-") << originLiteral.var()->varNumber;
		output << "->" << (cur.pos()?"":"-") << cur.var()->varNumber;
        output << ";\n";*/
	}
	cout << "lol" << toDraw.size() << endl;
    output << "}\n";	
	/*const Literal last = toDraw.top();
	curExamined = last.var()->getOriginClause()->getLiterals();
	const Literal originLiteral = last.var()->getOriginClause()->getRemaining();
	output << (originLiteral.pos()?"":"-") << originLiteral.var()->varNumber;
	output << "->" << (last.pos()?"":"-") << last.var()->varNumber;*/
}
	
#endif         

    


