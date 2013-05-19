#ifndef PARSER_HH
#define PARSER_HH


#include <iostream>
#include <stack>
#include <queue>
#include <string>
#include <utility>
#include <cctype>
#include <sstream>




inline void getParenthesisPos(const std::string &formula, std::queue<std::pair<int, int> > &parPos)
{
    std::stack<int> openingParPos;
    int lastOpeningParPos = -1;
    for (unsigned pos = 0; pos < formula.size(); pos++)
    {
        const char c = formula[pos];

        if (c == '(')
        {
            openingParPos.push(pos);
        }
        else if (c == ')')
        {
            if (openingParPos.empty())
            {
                std::cerr << "PROBLEME" << std::endl;
            }
            else
            {
                lastOpeningParPos = openingParPos.top();
                openingParPos.pop();

                parPos.push(std::make_pair(lastOpeningParPos, pos));
            }
        }
    }
}



inline void getTermParenthesisPos(const std::string &formula, std::queue<std::pair<int, int> > &parPos, const unsigned level)
{
    std::stack<int> openingParPos;
    int lastOpeningParPos = -1;
    for (unsigned pos = 0; pos < formula.size(); pos++)
    {
        const char c = formula[pos];

        if (c == '(')
        {
            openingParPos.push(pos);
        }
        else if (c == ')')
        {
            if (openingParPos.empty())
            {
                std::cerr << "PROBLEME" << std::endl;
            }
            lastOpeningParPos = openingParPos.top();
            openingParPos.pop();
            
            if (openingParPos.size() == level)
            {
                parPos.push(std::make_pair(lastOpeningParPos, pos));
            }
        }
    }
}






inline void getEqualitiesPos(const std::string &formula, std::queue<std::pair<int, int> >&parPos, std::queue<std::pair<int, int> > &posOfEqualities, std::queue<bool> &isDisequality)
{
    for (unsigned pos = 0; pos < formula.size(); pos++)
    {
        const char c = formula[pos];
        if (c == '=')
        {
            const int posPrev = (formula[pos-1] == '!' ? (pos-2):(pos-1));
            if (posPrev == pos-2)
                isDisequality.push(true);
            else
                isDisequality.push(false);

            int posBegin = posPrev, posEnd;

            /* À gauche du signe égal */
            /* Il s'agit d'un symbole de fonction */
            if (formula[posPrev] == ')')
            {
                while (parPos.front().second != posPrev)
                    parPos.pop();
                posBegin = parPos.front().first;
                parPos.pop();
            }
            while (posBegin > 0 && isalnum(formula[posBegin-1]))
                posBegin--;

            pos++;
            while (pos < formula.size()-1 && isalnum(formula[pos+1]))
                pos++;
            posEnd = pos;

            /* À droite du signe égal */
            /* Il s'agit d'un symbole de fonction */
            if (formula[pos+1] == '(')
            {
                while (parPos.front().first != pos+1)
                    parPos.pop();
                posEnd = parPos.front().second;
                parPos.pop();
            }
            posOfEqualities.push(std::make_pair(posBegin, posEnd));
        }

    }
}



inline std::string convertToPreTseitinFormula(const std::string &formula, std::queue<std::pair<int, int> > &posOfEqualities, std::queue<bool> &isDisequality)
{
    std::string ans;
    unsigned prevPos = -1;
    unsigned curNbVar = 0;
    while (!posOfEqualities.empty())
    {
        const int posDeb = posOfEqualities.front().first;
        const int posEnd = posOfEqualities.front().second;
        posOfEqualities.pop();

        std::stringstream foo;
        foo << curNbVar;
        ans += formula.substr(prevPos+1, posDeb - prevPos-1);
        if (isDisequality.front())
            ans += "~";
        isDisequality.pop();
        ans += ("c_" + foo.str());
        curNbVar++;
        prevPos = posEnd;
    }

    ans += formula.substr(prevPos+1, formula.size()-prevPos-1);
    ans += "\n";

    return ans;
}



#endif //PARSER_HH
