#ifndef PARSER_HH
#define PARSER_HH

#include <iostream>
#include <vector>

#include "Literal.hh"
#include "Clause.hh"



std::vector<Literal> parser_clause();


std::vector<Clause>  parser_sat();



#endif//PARSER_HH
