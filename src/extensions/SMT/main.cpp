#include <iostream>
#include <string>
#include <cctype>
#include <vector>
#include <map>
#include "UnionFind.hh"
#include "SMT.hh"
#include "Term.hh"


int main()
{
    SMT foo = SMT(std::cin);
    
    std::string formula = foo.getFormula();
    
    return 0;
}
