#ifndef PARSER_HH
#define PARSER_HH

#include <iostream>
#include <vector>

#include "Literal.hh"
#include "Clause.hh"

#include <istream>


void skip_comment();

void parser_header(const istream& input, unsigned int& nbr_var, unsigned int& nbr_clauses);

void parser_list_lit(const istream& input, std::vector<Literal>& res);



#endif//PARSER_HH
