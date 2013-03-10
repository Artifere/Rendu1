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
#ifdef RELEASE
        std::cout << "c Arg, entrée invalide " << std::endl;
#endif
        nbrVar = 0;
        nbrClauses = 0;
    }
    
}


void parserListLit(std::istream& input, std::vector<Literal>& ans, const unsigned int& limitVarNumber)
{
    int n;
    skipComment(input);
    while( (input >> n) && n )
    {
        unsigned int abs_n = (n < 0) ? -n : n;
        if(abs_n > limitVarNumber)
        {
#ifdef RELEASE
            std::cout <<"c Erreur de syntaxe dans l'entrée: "
                      <<"variable d'indice "<<abs_n<<" invalide "
                      <<"(l'indice doit être compris entre 1 et "<<limitVarNumber<<")."
                      <<std::endl;
            // le programme continue en ignorant la variable
            std::cout <<"c La variable est ignorée." << std::endl;
#endif
        }
        else
        {
            ans.push_back( Literal(abs_n-1, (n > 0)) );
        }
    }
}
