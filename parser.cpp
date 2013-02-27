#include "parser.hh"
#include <string>



void skip_comment(std::istream& input)
{
  // skip comments
  std::string read;
  while((input >> std::ws) && (std::cin.peek()=='c'))
    std::getline(input, read);
}


void parser_header(std::istream& input, unsigned int& nbr_var, unsigned int& nbr_clauses)
{
  std::string read;
  char c;
  skip_comment(input);
  if( !(std::cin >> c >> read >> nbr_var >> nbr_clauses) || (c != 'p') || (read != "cnf")) {
    std::cout << "c Arg, entrée invalide " << std::endl;
    nbr_var = 0;
    nbr_clauses = 0;
  }
}


void parser_list_lit(std::istream& input, std::vector<Literal>& res) {
  int n;
  skip_comment(input);
  while( (input >> n) && n ) {
    res.push_back( Literal((n < 0) ? (-n) : n, (n > 0)) );
  }
}
/*

std::vector<Clause>  parser_sat() {
  int nbVar, nbClauses;
  char c;
  std::string read;
  std::vector<Clause> res;
  

  // read the header
  if( !(cin >> c >> read >> nbVars >> nbClauses) || (c != p) || (read != "cnf")) {
    std::cout << "c Arg, entrée invalide " << std::endl;
  } else {
    while(nbClauses--) {
      // skip comments
      while((std::cin >> std::ws) && (cin.peek()=='c'))
        getline(std::cin, read);
      // read one clause
      res.push_back( Clause(parser_clause()) );
    }
  }
  
  return res;
}
*/

