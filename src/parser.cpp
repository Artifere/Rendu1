#include "parser.hh"
#include <string>


//Ignore les commentaires
void skipComment(std::istream& input)
{
    std::string read;
    while((input >> std::ws) && (input.peek()=='c'))
        std::getline(input, read);
}


// Parse l'en-tête du fichier d'entrée, et signale s'il y a une erreur de format
void parserHeader(std::istream& input, unsigned int& nbrVar, unsigned int& nbrClauses)
{
    std::string read;
    char c;
    skipComment(input);
    if( !(input >> c >> read >> nbrVar >> nbrClauses) || (c != 'p') || (read != "cnf"))
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
        // trouve la variable numero abs_s
        unsigned int abs_n = (n < 0) ? -n : n;
        std::vector<Variable*>::const_iterator it = addr.begin();
        while (it != addr.end())
        {
            if ((*it)->varNumber == abs_n)
                break;
            it ++;
        }
        if (it != addr.end())
            ans.push_back(Literal(*it, (n > 0)));
        else
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
    }
}


