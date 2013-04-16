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
	
	
	output << conflictLiteral.var()->varNumber << "->error;\n";
	output << "-" << conflictLiteral.var()->varNumber << "->error;\n";
   

    for (std::vector<Literal>::const_iterator it = curExamined.begin(); it != curExamined.end(); ++it)
    {
        if (it->var()->isFromCurBet(_stackBacktrack.back()))
            toDraw.push(*it);
    }
	
	seen[conflictLiteral.var()->varNumber] = true;
	
    while (toDraw.size() > 1)
    {
        const Literal cur = toDraw.top();
        toDraw.pop();
        std::cout << "DEBUG LOG : var = " << cur.var()->varNumber << ", clause = " << cur.var()->getOriginClause()->clauseNumber  << std::endl;
        
        curExamined = cur.var()->getOriginClause()->getLiterals();
        seen[cur.var()->varNumber] = true;
		
        for (std::vector<Literal>::const_iterator it = curExamined.begin(); it != curExamined.end(); ++it)
        {
            if (it->var()->isFromCurBet(_stackBacktrack.back()) && !seen[it->var()->varNumber])
            {
                seen[it->var()->varNumber] = true;
                toDraw.push(*it);
            }
        }
		
		const Literal originLiteral = cur.var()->getOriginClause()->getRemaining();
		output << (originLiteral.pos()?"":"-") << originLiteral.var()->varNumber;
		output << "->" << (cur.pos()?"":"-") << cur.var()->varNumber;
        output << ";\n";
	}
    output << "}\n";	
	/*const Literal last = toDraw.top();
	curExamined = last.var()->getOriginClause()->getLiterals();
	const Literal originLiteral = last.var()->getOriginClause()->getRemaining();
	output << (originLiteral.pos()?"":"-") << originLiteral.var()->varNumber;
	output << "->" << (last.pos()?"":"-") << last.var()->varNumber;*/
}
	
           

    


