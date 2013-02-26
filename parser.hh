#ifndef PARSER_HH
#define PARSER_HH

#include <iostream>
#include <vector>

#include "Literal.hh"
#include "Clause.hh"

#include <istream>


void skip_comment(std::istream& input);

void parser_header(std::istream& input, unsigned int& nbr_var, unsigned int& nbr_clauses);

void parser_list_lit(std::istream& input, std::vector<Literal>& res);



#endif//PARSER_HH
