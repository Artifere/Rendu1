#ifndef SMT_HH
#define SMT_HH

#include <istream>
#include <string>
#include <vector>
#include <queue>
#include <utility>
#include "Parser.hh"
#include "Term.hh"




class SMT
{
    public:
        inline SMT(std::istream &input);
        inline std::string getFormula(void) const
        {
            return _formula;
        }
        inline std::string getPreTseitinFormula(void) const
        {
            return _preTseitinFormula;
        }
        
        inline void buildTermsAndEqualitiesContent(std::queue<std::pair<int, int> > &posOfEqualities);

        inline void miscTests(void) const;

    private:
        std::vector<Term> _terms;
        std::vector<std::pair<int, int> > _equalitiesContent;
        std::string _formula;
        std::string _preTseitinFormula;
}; 


/* À des fins de debug */
inline void SMT::miscTests(void) const
{
    std::cout << "Voici les termes\n";
    for (std::vector<Term>::const_iterator it = _terms.begin(); it != _terms.end(); ++it)
    {
        std::cout << it->getStr() << "==>" << it->getId() << std::endl;
        std::cout << "Les sous-termes sont :\n";
        const std::vector<unsigned> subs = it->getSubterms();
        for (std::vector<unsigned>::const_iterator itSub = subs.begin(); itSub != subs.end(); ++itSub)
            std::cout << "\t" << _terms[*itSub].getStr() << std::endl;
        std::cout << "Fin des sous-termes\n\n";
    }


    std::cout << "Maintenant les égalités :\n";
    for (std::vector<std::pair<int, int> >::const_iterator itEq = _equalitiesContent.begin(); itEq != _equalitiesContent.end(); itEq++)
        std::cout << "c_" << (itEq-_equalitiesContent.begin()) << " ==> " << _terms[itEq->first].getStr() << " =  " << _terms[itEq->second].getStr() << "\n";


    std::cout << "\nHéhéhé :-)\n";
}


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


    std::queue<std::pair<int, int> > posOfEqualitiesCopy = posOfEqualities;
    /* Enfin on remplace chaque (dis)égalité par une « variable » */
    _preTseitinFormula = convertToPreTseitinFormula(_formula, posOfEqualities, isDisequality);

    //DEBUG
    std::cout << _preTseitinFormula << std::endl;


    /* On extrait les termes et remplit les égalités */
    
    buildTermsAndEqualitiesContent(posOfEqualitiesCopy);
}




inline void SMT::buildTermsAndEqualitiesContent(std::queue<std::pair<int, int > > &posOfEqualities)
{
    std::map<std::string, unsigned> alreadyBuiltTerms;
    while (!posOfEqualities.empty())
    {
        const unsigned begPos = posOfEqualities.front().first, endPos = posOfEqualities.front().second;
        posOfEqualities.pop();
        unsigned midPosBeg = begPos, midPosEnd;
        

        /* On cherche la position du signe égal, pour pouvoir séparer l'égalité en deux termes */
        while (_formula[midPosBeg] != '=')
            midPosBeg++;

        midPosEnd = midPosBeg+1;
        midPosBeg--;
        if (_formula[midPosBeg] == '!')
            midPosBeg--;
        


        /* On construit les termes, uniquement s'ils n'ont pas été déjà construits */
        const std::string str1 = _formula.substr(begPos, midPosBeg-begPos+1), str2 = _formula.substr(midPosEnd, endPos-midPosEnd+1);
        const std::map<std::string, unsigned>::iterator f1 = alreadyBuiltTerms.find(str1);
        const unsigned t1Id = (f1 == alreadyBuiltTerms.end() ? Term(alreadyBuiltTerms, _terms, str1).getId() : f1->second);
        
        const std::map<std::string, unsigned>::iterator f2 = alreadyBuiltTerms.find(str2);
        const unsigned t2Id = (f2 == alreadyBuiltTerms.end() ? Term(alreadyBuiltTerms, _terms, str2).getId() : f2->second);
        
        _equalitiesContent.push_back(std::make_pair(t1Id, t2Id));
    }

   
}


#endif //SMT_HH
