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
        Term(std::map<std::string, unsigned> &alreadyBuilt, unsigned *nextFreeId, const std::string term);




    private:
        unsigned _id;
        std::vector<unsigned> _subTerms;
        std::string _term;
};




#endif //TERM_HH
