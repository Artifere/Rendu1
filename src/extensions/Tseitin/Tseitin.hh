#ifndef EXPR_TREE_HH
#define EXPR_TREE_HH

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <utility>

#include "Parser.hh"



typedef std::pair<unsigned, bool> literal;
typedef std::vector<literal> clause;





unsigned ClauseTseitin(std::istream& in, std::vector<clause>& listClause, std::vector<std::pair<std::string,unsigned> >& varNumbers);




#endif//EXPR_TREE_HH
