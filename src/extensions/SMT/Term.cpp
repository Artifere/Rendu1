#include <map>
#include <string>
#include <queue>
#include <utility>
#include <cctype>
#include <vector>
#include "Term.hh"
#include "Parser.hh"




Term::Term(std::map<std::string, unsigned> &alreadyBuilt, std::vector<Term> &termsList, std::string term)
{
    if (!isFunction(term))
    {
        _str = term;
        _id = termsList.size();
    }


    else
    {
        std::queue<std::pair<int, int> > parPos;
        getTermParenthesisPos(term, parPos, 1);


        unsigned endFuncName = 0;
        while (term[endFuncName] != '(')
            endFuncName++;
        
        for (unsigned pos = endFuncName+1; pos < term.size()-1; pos++)
        {
            while (pos < term.size() && !isalpha(term[pos]))
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
                curEnd = pos;
            }
            pos = curEnd;
            const std::string subTermStr = term.substr(curBeg, curEnd-curBeg+1);
            unsigned subId;
            std::map<std::string, unsigned>::const_iterator findIt = alreadyBuilt.find(subTermStr);
            if (findIt == alreadyBuilt.end())
            {
                const Term sub = Term(alreadyBuilt, termsList, subTermStr);
                alreadyBuilt[sub._str] = termsList.size();
                subId = sub._id;
                termsList.push_back(sub);
            }
            else
            {
                subId = findIt->second;
            }
            _subTerms.push_back(subId);
        }
        _id = termsList.size();
        _str = term;
    }
}
