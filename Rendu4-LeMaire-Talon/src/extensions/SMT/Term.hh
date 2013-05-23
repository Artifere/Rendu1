#ifndef TERM_HH
#define TERM_HH



#include <vector>
#include <string>
#include <queue>
#include <set>
#include <map>
#include <utility>
#include "UnionFind.hh"
#include "Parser.hh"




class Term
{
    public:
        Term() {}
        Term(std::map<std::string, unsigned> &alreadyBuilt, std::vector<Term> &termsList, const std::string term);

        inline unsigned getId(void) const
        {
            return _id;
        }

        inline const std::string &getStr(void) const
        {
            return _str;
        }

        inline const std::vector<unsigned> &getSubterms(void) const
        {
            return _subTerms;
        }
        
        inline unsigned getNbrSubterms(void) const
        {
            return _subTerms.size();
        }

        inline bool isCongruent(const Term &t, UnionFind &congr) const;
    private:
        unsigned _id;
        std::vector<unsigned> _subTerms;
        std::string _str;
};


/* Renvoie vrai si les termes sont des "fonctions", ont le même nom, et des arguments congruents 2 à 2 */
inline bool Term::isCongruent(const Term &t2, UnionFind &congr) const
{
    if (!isFunction(_str) || !isFunction(t2._str) || _subTerms.size() != t2._subTerms.size())
        return false;

    std::string name, t2Name;
    for (unsigned i = 0; _str[i] != '('; i++)
        name.push_back(_str[i]);

    for (unsigned i = 0; t2._str[i] != '('; i++)
        t2Name.push_back(t2._str[i]);

    if (name != t2Name)
        return false;
   
    for (unsigned i = 0; i < _subTerms.size(); i++)
        if (congr.find(_subTerms[i]) != congr.find(t2._subTerms[i]))
            return false;

    return true;
}

#endif //TERM_HH
