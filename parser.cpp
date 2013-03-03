#include "parser.hh"
#include <string>



void skipComment(std::istream& input)
{
    // skip comments
    std::string read;
    while((input >> std::ws) && (std::cin.peek()=='c'))
        std::getline(input, read);
}


void parserHeader(std::istream& input, unsigned int& nbrVar, unsigned int& nbrClauses)
{
    std::string read;
    char c;
    skipComment(input);
    if( !(std::cin >> c >> read >> nbrVar >> nbrClauses) || (c != 'p') || (read != "cnf"))
    {
        std::cout << "c Arg, entrée invalide " << std::endl;
        nbrVar = 0;
        nbrClauses = 0;
    }
    
}


void parserListLit(std::istream& input, std::vector<Literal>& ans)
{
    int n;
    skipComment(input);
    while( (input >> n) && n )
    {
        ans.push_back( Literal(((n < 0) ? (-n-1) : n-1), (n > 0)) );
    }
}
/*

std::vector<Clause>  parser_sat() {
  int nbVar, nbClauses;
  char c;
  std::string read;
  std::vector<Clause> ans;


  // read the header
  if( !(cin >> c >> read >> nbVars >> nbClauses) || (c != p) || (read != "cnf")) {
    std::cout << "c Arg, entrée invalide " << std::endl;
  } else {
    while(nbClauses--) {
      // skip comments
      while((std::cin >> std::ws) && (cin.peek()=='c'))
        getline(std::cin, read);
      // read one clause
      ans.push_back( Clause(parser_clause()) );
    }
  }

  return ans;
}
*/

