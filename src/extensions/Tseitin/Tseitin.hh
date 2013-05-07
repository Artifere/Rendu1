#ifndef EXPR_TREE_HH
#define EXPR_TREE_HH

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "Parser.hh"



typedef std::pair<unsigned, bool> literal;
typedef std::vector<literal> clause;




unsigned ClauseTseitin(Parser & parser, std::vector<clause>& listClause);




#endif//EXPR_TREE_HH
