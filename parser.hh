#ifndef PARSER_HH
#define PARSER_HH

#include <iostream>
#include <vector>
#include "Literal.hh"
#include "Variable.hh"
#include <istream>


void skipComment(std::istream& input);

void parserHeader(std::istream& input, unsigned int& nbrVar, unsigned int& nbrClauses);

void parserListLit(std::istream& input, std::vector<Literal>& ans, const std::vector<Variable*>& addr);



#endif //PARSER_HH defined
