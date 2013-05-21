#include <map>
#include <string>
#include <queue>
#include <utility>
#include <cctype>
#include <vector>
#include "Term.hh"
#include "Parser.hh"



/* Invariant du constructeur : il n'est jamais appelé sur un terme déjà construit (ie dans alreadyBuilt)
   Le constructeur ajoute le terme et ses sous-termes  à alreadyBuilt et à termsList */
Term::Term(std::map<std::string, unsigned> &alreadyBuilt, std::vector<Term> &termsList, std::string term)
{
    /* Si on a un symbole de fonction, il faut énumérer les sous-termes */
    if (isFunction(term))
    {
        std::queue<std::pair<int, int> > parPos;
        getTermParenthesisPos(term, parPos, 1);


        unsigned endFuncName = 0;
        while (term[endFuncName] != '(')
            endFuncName++;
        
        for (unsigned pos = endFuncName+1; pos < term.size()-1; pos++)
        {
            while (pos < term.size() && !isalnum(term[pos]))
                pos++;

            unsigned curBeg = pos, curEnd;
            while (!parPos.empty() && parPos.front().first <= pos)
            {
                parPos.pop();
            }

            if (!parPos.empty() && parPos.front().first == pos+1)
            {
                curEnd = parPos.front().second;
                parPos.pop();
            }
            else
            {
                while (isalnum(term[pos+1]))
                    pos++;
                curEnd = pos;
            }
            
            const std::string subTermStr = term.substr(curBeg, curEnd-curBeg+1);
            unsigned subId;
            std::map<std::string, unsigned>::const_iterator findIt = alreadyBuilt.find(subTermStr);
            
            /* Le sous-terme est nouveau, on le construit */
            if (findIt == alreadyBuilt.end())
            {
                const Term sub = Term(alreadyBuilt, termsList, subTermStr);
                subId = sub._id;
            }
            else
            {
                subId = findIt->second;
            }
            _subTerms.push_back(subId);
        }
    }
    _id = termsList.size();
    _str = term;
    
    termsList.push_back(*this);
    alreadyBuilt[term] = _id;
}
