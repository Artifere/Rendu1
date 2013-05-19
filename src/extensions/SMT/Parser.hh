#include <iostream>
#include <stack>
#include <queue>
#include <string>
#include <utility>
#include <cctype>
#include <sstream>

inline std::string convertToBasicLogic(std::istream& formula, void* corres)
{
    std::stack<int> openingParPos;
    std::queue<std::pair<int, int> > parPos;


    std::string formulaS;
    while ((formula >> std::ws) && !formula.eof())
    {
        char c;
        formula >> c;
        formulaS += c;
     }

    int pos = 0;
    int lastOpeningParPos = -1;
    char prevC = -1;
    for (unsigned pos = 0; pos < formulaS.size(); pos++)
    {
        const char c = formulaS[pos];
        
        if (c == '(')
        {
            openingParPos.push(pos);
        }
        else if (c == ')')
        {
            if (openingParPos.empty())
            {
                //alalala pas bien : formule invalide
            }
            else
            {
                lastOpeningParPos = openingParPos.top();
                openingParPos.pop();

                parPos.push(std::make_pair(lastOpeningParPos, pos));
            }
        }

        pos++;
    }

    

    std::queue<std::pair<int, int> > posOfEqualities;
    for (unsigned pos = 0; pos < formulaS.size(); pos++)
    {
        const char c = formulaS[pos];
        if (c == '=')
        {
            const int posPrev = (formulaS[pos-1] == '!' ? (pos-2):(pos-1));
            while (parPos.front().second != posPrev)
                parPos.pop();
            int posBegin = parPos.front().first;
            parPos.pop();
            while (posBegin > 0 && isalpha(formulaS[posBegin-1]))
                posBegin--;


            while (parPos.front().first <= pos+1)
                parPos.pop();
            int posEnd = parPos.front().second;
            parPos.pop();
            
            posOfEqualities.push(std::make_pair(posBegin, posEnd));
        }
    }

    std::string res;

    unsigned prevPos = -1;
    unsigned curNbVar = 0;
    while (!posOfEqualities.empty())
    {
        const int posDeb = posOfEqualities.front().first;
        const int posEnd = posOfEqualities.front().second;
        posOfEqualities.pop();

        std::stringstream foo;
        foo << curNbVar;
        res = res + formulaS.substr(prevPos+1, posDeb - prevPos-1) + "c_" + foo.str();
        curNbVar++;
        prevPos = posEnd;
    }

    res += formulaS.substr(prevPos+1, formulaS.size()-prevPos-1);

    return res;
}
        






                




