#include "parser.hh"
#include <string>


std::vector<Literal> parser_clause() {
  std::vector<Literal> res();
  int n;
  while( (std::cin >> n) && n ) {
    res.push_back( Literal((n < 0) ? (-n) : n, (n > 0));
  }
  return res;
}


std::vector<Clause>  parser_sat() {
  int nbVar, nbClauses;
  char c;
  std::string read;
  std::vector<Clause> res;
  
  // skip comments
  while((std::cin >> std::ws) && (cin.peek()=='c'))
    getline(std::cin, read);

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


