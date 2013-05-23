#ifndef SOLUTION_HH
#define SOLUTION_HH

#include <set>
#include <utility>
#include "UnionFind.hh"
#include "Parser.hh"

class Solution
{
    public:
        inline Solution(SMT& problem):_problem(problem),_eqData(problem.getNbrTerms())
        {
            std::vector<bool> assignations = problem.getAssignations();
            for (unsigned i = 0; i < assignations.size(); i++)
            {
                if (assignations[i])
                {
                    _eqIdLeftList.push(i);
                    _eqList.push(problem.getEqContent(i));
                }
                else
                {
                    _diseqIdLeftList.push(i);
                    _diseqList.push(problem.getEqContent(i));
                }
            }
        }
        
        inline bool addEqualities(void);        
        inline bool addDisequalities(void);
        inline bool check(void)
        {
            return addEqualities() && addDisequalities();
        }
        
        inline void printNewClause(std::ostream &outProblem);

    private:
        SMT &_problem;
        UnionFind _eqData;
        std::queue<std::pair<unsigned, unsigned> > _diseqList, _eqList;
		/*
		   Ces files servent à repérer quelles variables on a lues pour pouvoir
		   renvoyer une clause expliquand le conflit un peu meilleure que la négation
		   de l'assignation totale
		*/
        std::queue<unsigned> _diseqIdDoneList, _eqIdDoneList, _eqIdLeftList, _diseqIdLeftList;
        std::stack<std::pair<unsigned, unsigned> > _eqImpliedList; /* Liste d'égalités non présente dans la formule, mais déduites */
};


/* On traite toutes les égalités, en clôturant par congruence en même temps */
inline bool Solution::addEqualities(void)
{
    while (!_eqList.empty() || !_eqImpliedList.empty())
    {
        bool wasImplied = false;
        unsigned idT1;
        unsigned idT2;

        if (!_eqImpliedList.empty())
        {
            idT1 = _eqImpliedList.top().first;
            idT2 = _eqImpliedList.top().second;
            _eqImpliedList.pop();
            wasImplied = true;
        }

        else
        {
            idT1 = _eqList.front().first;
            idT2 = _eqList.front().second;
        }

        const unsigned s1 = _eqData.find(idT1);
        const unsigned s2 = _eqData.find(idT2);
        /* Cette égalité n'apporte rien de plus : inutile de l'inclure dans la clause de conflit */
        if (s1 == s2)
        {
            _eqIdLeftList.pop();
            _eqList.pop();

            continue;
        }



        _eqData.unionAfterFind(s1, s2);
        const Term &t1 = _problem.getTerm(idT1), &t2 = _problem.getTerm(idT2);

        /* Si on a deux symboles de fonction, on doit vérifier que ce sont les mêmes, qu'ils ont
           la même arité, et que les sous-termes sont congrus. */
        if (!t1.getSubterms().empty() && !t2.getSubterms().empty())
        {
            std::string t1Name, t2Name;
            const std::string &str1 = t1.getStr(), &str2 = t2.getStr();
            for (unsigned i = 0; str1[i] != '('; i++)
                t1Name.push_back(str1[i]);

            for (unsigned i = 0; str2[i] != '('; i++)
                t2Name.push_back(str2[i]);

            if (t1Name != t2Name)
                return false;

            else
            {
                const std::vector<unsigned> &sub1 = t1.getSubterms(), &sub2 = t2.getSubterms();
                if (sub1.size() != sub2.size())
                    return false;
                for (unsigned i = 0; i < sub1.size(); i++)
                    _eqList.push(std::make_pair(sub1[i], sub2[i]));
            }
        }

        /* Peut-être qu'avec les nouvelles informations des termes sont devenus congruents */
        const unsigned nbrTerms = _problem.getNbrTerms();
        for (unsigned it1 = 0; it1 < nbrTerms; it1++)
            for (unsigned it2 = it1+1; it2 < nbrTerms; it2++)
                if (_eqData.find(it1) != _eqData.find(it2) && _problem.getTerm(it1).isCongruent(_problem.getTerm(it2), _eqData))
                    _eqImpliedList.push(std::make_pair(it1, it2));




        if (!wasImplied) /* On garde une trace de cette égalité, qui vient de la formule */
        {
            _eqIdDoneList.push(_eqIdLeftList.front());
            _eqIdLeftList.pop();
            _eqList.pop();
        }
    }

    return true;
}


/* Pour les diségalités, on vérifie que ça ne rentre pas en conflit avec des déductions de congruences
   déjà faites. Aussi, on vérifie qu'on n'a pas de chose du genre de "t = x /\ g(t) != g(x)" */
inline bool Solution::addDisequalities(void)
{
    while (!_diseqList.empty())
    {
        unsigned idT1 = _diseqList.front().first;
        unsigned idT2 = _diseqList.front().second;
        
        if (_eqData.find(idT1) == _eqData.find(idT2))
            return false;

        const Term &t1 = _problem.getTerm(idT1), &t2 = _problem.getTerm(idT2);
        if (!t1.getSubterms().empty() && !t2.getSubterms().empty())
        {
            std::string t1Name, t2Name;
            const std::string &str1 = t1.getStr(), &str2 = t2.getStr();
            for (unsigned i = 0; str1[i] != '('; i++)
                t1Name.push_back(str1[i]);

            for (unsigned i = 0; str2[i] != '('; i++)
                t2Name.push_back(str2[i]);

            if (t1Name == t2Name)
            {
                const std::vector<unsigned> &sub1 = t1.getSubterms(), &sub2 = t2.getSubterms();
                if (sub1.size() == sub2.size())
                {
                    bool areEqual = true;
                    for (unsigned i = 0; areEqual && i < sub1.size(); i++)
                        areEqual = (_eqData.find(sub1[i]) == _eqData.find(sub2[i]));
                    if (areEqual)
                        return false;
                }
            }
        }
        _diseqIdDoneList.push(_diseqIdLeftList.front());
        _diseqIdLeftList.pop();
        _diseqList.pop();
    }


    return true;
}


/* On écrit la clause de conflit. Elle consiste en une disjonction des négations des états des variables (d'égalités ou diségalités)
   qu'on a lues jusqu'à présent : la contradiction ne vient pas d'une variable non encore lue. */
inline void Solution::printNewClause(std::ostream &outProblem)
{
	/* L'erreur est survenue en ajoutant une égalité */
    if (!_eqIdLeftList.empty())
    {
        const unsigned last = _eqIdLeftList.front();
        const unsigned lastInCnf = _problem.getCnfMatchVar(last);
        const bool prevVal = _problem.getAssignations(last);
        
        if (prevVal)
            outProblem << '-';
        outProblem << lastInCnf;
        
        while (!_eqIdDoneList.empty())
        {
            outProblem << " ";
            const unsigned cur = _eqIdDoneList.front();
            const unsigned curInCnf = _problem.getCnfMatchVar(cur);
            const bool prevVal = _problem.getAssignations(cur);
            
            if (prevVal)
                outProblem << '-';

            outProblem << curInCnf;
            _eqIdDoneList.pop();
        }
    }

	/* L'erreur est survenue en ajoutant une diségalité */
    else
    {
        const unsigned last = _diseqIdLeftList.front();
        const unsigned lastInCnf = _problem.getCnfMatchVar(last);
        const bool prevVal = _problem.getAssignations(last);
        
        if (prevVal)
            outProblem << '-';
        outProblem << lastInCnf;


        while (!_diseqIdDoneList.empty())
        {
            outProblem << " ";
            const unsigned cur = _diseqIdDoneList.front();
            const unsigned curInCnf = _problem.getCnfMatchVar(cur);
            const bool prevVal = _problem.getAssignations(cur);
            
            if (prevVal)
                outProblem << '-';

            outProblem << curInCnf;
            _diseqIdDoneList.pop();
        }
        
        while (!_eqIdDoneList.empty())
        {
            outProblem << " ";
            const unsigned cur = _eqIdDoneList.front();
            const unsigned curInCnf = _problem.getCnfMatchVar(cur);
            const bool prevVal = _problem.getAssignations(cur);
            
            if (prevVal)
                outProblem << '-';

            outProblem << curInCnf;
            _eqIdDoneList.pop();
        }
    }
    outProblem << " 0"<< std::endl;
}



#endif //SOLUTION_HH
