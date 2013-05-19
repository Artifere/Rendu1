#include <istream>
#include <string>
#include <vector>
#include <queue>
#include <utility>
#include "Parser.hh"


#ifndef SMT_HH
#define SMT_HH

class SMT
{
    public:
        inline SMT(std::istream &input);




    private:
        std::vector<std::string> _terms;
        std::vector<std::pair<int, int> > _equalitiesContent;
        std::string _formula;
        std::string _preTseitinFormula;
}; 


/* Transforme le flux passé en entrée en une chaîne de caractères équivalente,
   dans laquelle les égalités/négations d'égalités ont été remplacées par des
   variables de la forme c_nombre. On peut ensuite passer cette chaîne à
   Tseitin pour avoir un cnf. */


inline SMT::SMT(std::istream &input)
{
    while ((input >> std::ws) && !input.eof())
    {
        char c;
        input >> c;
        _formula += c;
    }


    /* On commence par trouver tous les couples des positions des couples de
       parenthèses ouvrantes et fermantes */
    std::queue<std::pair<int, int> > parPos;
    getParenthesisPos(_formula, parPos); 


    /* À présent, pour chaque (dis)égalité, on cherche la zone coranspondant
       à cette (dis)égalité, pour la supprimer de la chaîne finale */
    std::queue<std::pair<int, int> > posOfEqualities;
    std::queue<bool> isDisequality;
    getEqualitiesPos(_formula, parPos, posOfEqualities, isDisequality);


    /* Enfin on remplace chaque (dis)égalité par une « variable » */
    _preTseitinFormula = getPreTseitinFormula(_formula, posOfEqualities, isDisequality);

    //DEBUG
    std::cout << _preTseitinFormula << std::endl;
}

#endif //SMT_HH
