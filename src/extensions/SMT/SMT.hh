#ifndef SMT_HH
#define SMT_HH

#include <istream>
#include <string>
#include <vector>
#include <queue>
#include <utility>
#include <algorithm>
#include "Parser.hh"
#include "Term.hh"




class SMT
{
    public:
        inline SMT(std::istream &input);
        inline std::string getFormula(void) const
        {
            return _originalFormula;
        }
        inline std::string getPreTseitinFormula(void) const
        {
            return _preTseitinFormula;
        }
        inline unsigned getNbrEqualities(void) const
        {
            return _nbrEq;
        }


        inline void buildTermsAndEqualitiesContent(std::queue<std::pair<int, int> > &posOfEqualities);
        inline void buildAppearing(void);

        inline void miscTests(void) const;


        inline void readSolverAssignations(std::istream& solverRes);


        inline void printResult(void)
        {
            std::cout << "Et voici le résultat !\n";
            for (unsigned eq = 0; eq < _nbrEq; eq++)
            {
                std::cout << _terms[_equalitiesContent[eq].first].getStr() << " ";
                if (!_assignations[eq])
                    std::cout << '!';
                std::cout << "= " << _terms[_equalitiesContent[eq].second].getStr() << std::endl;
            }
        }

    private:
        std::vector<Term> _terms;
        std::vector<std::pair<int, int> > _equalitiesContent;
        std::string _originalFormula;
        std::string _preTseitinFormula;
        std::vector<std::vector<unsigned> > _termAppearsIn;
        unsigned _nbrTerms, _nbrEq;

        std::map<unsigned, unsigned> _cnfVarMatch; /* Correspondance entre les variables du cnf et les c_i */
        std::vector<bool> _assignations;
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

        std::cout << "Ce terme apparaît dans les termes suivants :\n";
        for (std::vector<unsigned>::const_iterator itApp = _termAppearsIn[it->getId()].begin(); itApp != _termAppearsIn[it->getId()].end(); ++itApp)
            std::cout << "\t\t" << _terms[*itApp].getStr() << "\n";
        std::cout << "Fin des apparitions\n\n\n";
    }


    std::cout << "Maintenant les égalités :\n";
    for (std::vector<std::pair<int, int> >::const_iterator itEq = _equalitiesContent.begin(); itEq != _equalitiesContent.end(); itEq++)
        std::cout << "c_" << (itEq-_equalitiesContent.begin()) << " ==> " << _terms[itEq->first].getStr() << " =  " << _terms[itEq->second].getStr() << "\n";


    std::cout << "Les correspondances de variable dans le cnf sont :\n";
    for (std::map<unsigned, unsigned>::const_iterator it = _cnfVarMatch.begin(); it != _cnfVarMatch.end(); ++it)
        std::cout << it->first << "==>" << it->second << std::endl;

    std::cout << "\nHéhéhé :-)\n";
}



/* Transforme le flux passé en entrée en une chaîne de caractères équivalente,
   dans laquelle les égalités/négations d'égalités ont été remplacées par des
   variables de la forme c_nombre. On peut ensuite passer cette chaîne à
   Tseitin pour avoir un cnf.
   De plus, remplit les différents champs utiles pour un problème SMT.
 */
inline SMT::SMT(std::istream &input)
{
    while ((input >> std::ws) && !input.eof())
    {
        char c;
        input >> c;
        _originalFormula += c;
    }


    /* On commence par trouver tous les couples des positions des couples de
       parenthèses ouvrantes et fermantes */
    std::queue<std::pair<int, int> > parPos;
    getParenthesisPos(_originalFormula, parPos); 


    /* À présent, pour chaque (dis)égalité, on cherche la zone coranspondant
       à cette (dis)égalité, pour la supprimer de la chaîne finale */
    std::queue<std::pair<int, int> > posOfEqualities;
    std::queue<bool> isDisequality;
    getEqualitiesPos(_originalFormula, parPos, posOfEqualities, isDisequality);


    std::queue<std::pair<int, int> > posOfEqualitiesCopy = posOfEqualities;
    /* Enfin on remplace chaque (dis)égalité par une « variable » */
    _preTseitinFormula = convertToPreTseitinFormula(_originalFormula, posOfEqualities, isDisequality);


    /* On extrait les termes et remplit les égalités */
    buildTermsAndEqualitiesContent(posOfEqualitiesCopy);
    _nbrEq = _equalitiesContent.size();
    _assignations.resize(_nbrEq);

    _nbrTerms = _terms.size();
    /* On construit le tableau qui contient, pour tout terme t, la liste des termes de la forme
       f(t1, ..., tn) où l'un des ti est égal à t */
    buildAppearing();


    /* On envoie la formule à Tseitin, puis on récupère ce qu'il renvoie. On fait la correspondance entre
       les variables du problème cnf et les c_i, puis on réécrit le problème cnf sans son les lignes qui
       indiquent cette correspondance.
     */
    std::ofstream outPreTseitin;
    outPreTseitin.open("fooPreTseitin.out");
    outPreTseitin << _preTseitinFormula;
    outPreTseitin.close();
    system("../Tseitin/tseitin < fooPreTseitin.out > tseitinized.cnf");
    std::ifstream inTseitinized;
    inTseitinized.open("tseitinized.cnf");


    std::string fooS;
    std::getline(inTseitinized, fooS);
    std::getline(inTseitinized, fooS);


    char fooC;
    for (unsigned i = 0; i < _nbrEq; i++)
    {
        unsigned n1, n2;
        inTseitinized >> fooC >> fooC >> fooC >> n1 >> fooC >> n2;
        std::cout << n1 << ">>" << n2 << std::endl;
        _cnfVarMatch[n2] = n1;
    }
    std::ofstream outProblem;
    outProblem.open("problem.cnf");



    while (!inTseitinized.eof() && getline(inTseitinized, fooS))
        outProblem << fooS << '\n';

    inTseitinized.close();
    outProblem.close();
    system("rm fooPreTseitin.out");
    system("rm tseitinized.cnf");




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
        while (_originalFormula[midPosBeg] != '=')
            midPosBeg++;

        midPosEnd = midPosBeg+1;
        midPosBeg--;
        if (_originalFormula[midPosBeg] == '!')
            midPosBeg--;
        


        /* On construit les termes, uniquement s'ils n'ont pas été déjà construits */
        const std::string str1 = _originalFormula.substr(begPos, midPosBeg-begPos+1), str2 = _originalFormula.substr(midPosEnd, endPos-midPosEnd+1);
        const std::map<std::string, unsigned>::iterator f1 = alreadyBuiltTerms.find(str1);
        const unsigned t1Id = (f1 == alreadyBuiltTerms.end() ? Term(alreadyBuiltTerms, _terms, str1).getId() : f1->second);
        
        const std::map<std::string, unsigned>::iterator f2 = alreadyBuiltTerms.find(str2);
        const unsigned t2Id = (f2 == alreadyBuiltTerms.end() ? Term(alreadyBuiltTerms, _terms, str2).getId() : f2->second);
        
        _equalitiesContent.push_back(std::make_pair(t1Id, t2Id));
    }

   
}



/* Construit le tableau qui contient, pour tout terme t, la liste des termes de la forme
       f(t1, ..., tn) où l'un des ti est égal à t */
inline void SMT::buildAppearing(void)
{
    for (unsigned termId = 0; termId < _nbrTerms; termId++)
    {
        std::vector<unsigned> curApp, curSubTermList;
        for (unsigned term2Id = 0; term2Id < _nbrTerms; term2Id++)
        {
            curSubTermList = _terms[term2Id].getSubterms();

            if (std::find(curSubTermList.begin(), curSubTermList.end(), termId) != curSubTermList.end())
                curApp.push_back(term2Id);
        }
        _termAppearsIn.push_back(curApp);
    }
}




inline void SMT::readSolverAssignations(std::istream& solverRes)
{
    char fooC;

    for (unsigned i = 0; i < _nbrEq; i++)
    {
        unsigned var;
        bool isNeg = false;
        solverRes >> fooC;
        if (solverRes.peek() == '-')
        {
            isNeg = true;
            solverRes >> fooC;
        }
        solverRes >> var;
        const unsigned idVarMatch = _cnfVarMatch[abs(var)];
        _assignations[idVarMatch] = !isNeg;
    }
}


#endif //SMT_HH

