#include <iostream>
#include <stack>
#include <queue>
#include <string>
#include <utility>
#include <cctype>
#include <sstream>


/* Transforme le flux passé en entrée en une chaîne de caractèans équivalente,
   dans laquelle les égalités/négations d'égalités ont été remplacées par des
   variables de la forme c_nombre. On peut ensuite passer cette chaîne à
   Tseitin pour avoir un cnf. */

inline std::string convertToBasicLogic(std::istream& formula, void* corres)
{
    std::string formulaS;
    while ((formula >> std::ws) && !formula.eof())
    {
        char c;
        formula >> c;
        formulaS += c;
     }

    
    /* On commence par trouver tous les couples des positions des couples de
       parenthèses ouvrantes et fermantes */
    std::stack<int> openingParPos;
    std::queue<std::pair<int, int> > parPos;
    int pos = 0;
    int lastOpeningParPos = -1;
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


    /* À présent, pour chaque (dis)égalité, on cherche la zone coranspondant
       à cette (dis)égalité, pour la supprimer de la chaîne finale */
    std::queue<std::pair<int, int> > posOfEqualities;
    for (unsigned pos = 0; pos < formulaS.size(); pos++)
    {
        const char c = formulaS[pos];
        if (c == '=')
        {
            const int posPrev = (formulaS[pos-1] == '!' ? (pos-2):(pos-1));
            int posBegin = posPrev, posEnd;

            /* À gauche du signe égal */
            /* Il s'agit d'un symbole de fonction */
            if (formulaS[posPrev] == ')')
            {
                while (parPos.front().second != posPrev)
                    parPos.pop();
                posBegin = parPos.front().first;
                parPos.pop();
            }
            while (posBegin > 0 && isalpha(formulaS[posBegin-1]))
                posBegin--;
            
            pos++;
            while (pos < formulaS.size()-1 && isalpha(formulaS[pos+1]))
                pos++;
            posEnd = pos;

            /* À droite du signe égal */
            /* Il s'agit d'un symbole de fonction */
            if (formulaS[pos+1] == '(')
            {
                while (parPos.front().first != pos+1)
                    parPos.pop();
                posEnd = parPos.front().second;
                parPos.pop();
            }
            posOfEqualities.push(std::make_pair(posBegin, posEnd));
        }

    }


    /* Enfin on remplace chaque (dis)égalité par une « variable » */
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
        ans = ans + formulaS.substr(prevPos+1, posDeb - prevPos-1) + "c_" + foo.str();
        curNbVar++;
        prevPos = posEnd;
    }

    ans += formulaS.substr(prevPos+1, formulaS.size()-prevPos-1);
    ans += "\n";
    return ans;
}


