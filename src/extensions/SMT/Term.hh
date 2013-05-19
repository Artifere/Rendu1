#ifndef TERM_HH
#define TERM_HH



#include <vector>
#include <string>
#include <queue>
#include <set>
#include <map>
#include <utility>






class Term
{
    public:
        Term() {}
        Term(std::map<std::string, unsigned> &alreadyBuilt, std::vector<Term> &termsList, const std::string term);




    private:
        unsigned _id;
        std::vector<unsigned> _subTerms;
        std::string _str;
};


inline bool isFunction(const std::string s)
{
    for (int i = 0; i < s.size(); i++)
        if (s[i] == '(')
            return true;
    return false;
}

#endif //TERM_HH
