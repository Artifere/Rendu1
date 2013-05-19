#include <map>
#include <string>
#include <queue>
#include <utility>
#include <cctype>
#include <vector>
#include "Term.hh"
#include "Parser.hh"




Term::Term(std::map<std::string, unsigned> &alreadyBuilt, unsigned *nextFreeId, std::string term)
{
    std::queue<std::pair<int, int> > parPos;
    getTermParenthesisPos(term, parPos);

    for (unsigned pos = 0; pos < term.size(); pos++)
    {
        unsigned curDeb = pos;
        while (pos < term.size()-1 && isalnum(term[pos+1]))
            pos++;

        const std::string subTermStr = term.substr(curDeb, pos-curDeb);
        unsigned subId;
        std::map<std::string, unsigned>::const_iterator findIt = alreadyBuilt.find(subTermStr);
        if (findIt == alreadyBuilt.end())
        {
            Term *sub = new Term(alreadyBuilt, nextFreeId, subTermStr);
            subId = sub->_id;
        }
        else
        {
            subId = findIt->second;
        }
        _subTerms.push_back(subId);
    }


   alreadyBuilt[term] = *nextFreeId;
   (*nextFreeId)++;
}
