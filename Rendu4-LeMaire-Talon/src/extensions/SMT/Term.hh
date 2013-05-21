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

    private:
        unsigned _id;
        std::vector<unsigned> _subTerms;
        std::string _str;
};




#endif //TERM_HH
