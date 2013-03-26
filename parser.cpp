#include "parser.hh"
#include <string>


//Ignore les commentaires
void skipComment(std::istream& input)
{
    std::string read;
    while((input >> std::ws) && (std::cin.peek()=='c'))
        std::getline(input, read);
}


// Parse l'en-tête du fichier d'entrée, et signale s'il y a une erreur de format
void parserHeader(std::istream& input, unsigned int& nbrVar, unsigned int& nbrClauses)
{
    std::string read;
    char c;
    skipComment(input);
    if( !(std::cin >> c >> read >> nbrVar >> nbrClauses) || (c != 'p') || (read != "cnf"))
    {
        #if VERBOSE > 0
        std::cout << "c Arg, entrée invalide " << std::endl;
        #endif
        nbrVar = 0;
        nbrClauses = 0;
    }
}


// Parse le reste du fichier
void parserListLit(std::istream& input, std::vector<Literal>& ans, const std::vector<Variable*>& addr)
{
    int n;
    skipComment(input);
    while( (input >> n) && n )
    {
        unsigned int abs_n = (n < 0) ? -n : n;
        if(abs_n > addr.size())
        {
            #if VERBOSE > 0
            std::cout <<"c Erreur de syntaxe dans l'entrée: "
                      <<"variable d'indice "<<abs_n<<" invalide "
                      <<"(l'indice doit être compris entre 1 et "<<addr.size()<<")."
                      <<std::endl;
            // le programme continue en ignorant la variable
            std::cout <<"c La variable est ignorée." << std::endl;
            #endif
        }
        else
            ans.push_back(Literal(addr[abs_n-1], (n > 0)));
    }
}


