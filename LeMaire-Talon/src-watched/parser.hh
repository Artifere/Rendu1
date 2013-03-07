#ifndef PARSER_HH
#define PARSER_HH

#include <iostream>
#include <vector>
#include "Literal.hh"
#include "Clause.hh"
#include <istream>


void skipComment(std::istream& input);

void parserHeader(std::istream& input, unsigned int& nbrVar, unsigned int& nbrClauses);

void parserListLit(std::istream& input, std::vector<Literal>& ans, const unsigned int& limitVarNumber);



#endif//PARSER_HH
