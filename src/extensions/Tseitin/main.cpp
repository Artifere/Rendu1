#include <iostream>
#include "ExprTree.hh"
#include "Parser.hh"

#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <stdio.h>
#include <fstream>
#include <istream>
#include <sstream>


using namespace std;


// écrit dans write le problème cnf obtenu par transformation de Tseitin depuis le flux read
// renvoie l'assignation nom/numéro des variables
vector<pair<string,unsigned> > writeProbleme(istream& read, ostream& write);


// execute la commande call
// renvoie ce que le programme affiche sur sa sortie standard
string executeProg(const char* call);


int main(int argc, char * argv[])
{
    if (argc == 3 && string(argv[1]) == "-r")
    {
        //cerr << "c Écriture du problème cnf dans " << argv[2] << endl;
        fstream write(argv[2], ios_base::out | ios_base::trunc);
        const vector<pair<string,unsigned> > assoc(writeProbleme(cin, write));

        // appelle le solver
        string call = string("../.././release ")+argv[2];
        string sortieSolver = executeProg(call.c_str());
        //cout << "sortie du solver :" << out << endl << endl;     
   
        // parse la sortie du solver pour récupérer l'assignation
        istringstream fluxSortieSolver(sortieSolver, ios_base::in);
        vector<bool> assign(readAssignation(fluxSortieSolver, assoc.size()));
        
        // affiche l'assignation lue
        if (assign.empty()) {
            cout << "s UNSATISFIABLE" << endl;
        } else {
            cout << "s SATISFIABLE" << endl;
            for(vector<pair<string,unsigned> >::const_iterator it=assoc.begin(); it!=assoc.end(); ++it)
                cout << "v " << (assign[it->second] ? ' ' : '-') << it->first << endl;
        }
    }
    else if (argc == 1)
    {
        writeProbleme(cin,cout);
    }
    else
    {
        cerr << "Arguments invalides.\nLes arguments attendus sont :" << endl;
        cerr << "soit rien : le problème cnf correspondant à la formule est renvoyé." << endl;
        cerr << "soit -r <nom de fichier> : le problème cnf est écrit dans <nom de fichier>,"
             << "le sat solver est appellé sur ce problème, et l'assignation des variables est renvoyée (ou \"s UNSATISFIABLE\" si le problème est unsat)" << endl;
    }
    return 0;
}



vector<pair<string,unsigned> > writeProbleme(istream& read, ostream& write)
{
    vector<clause> clauses;
    std::vector<pair<std::string,unsigned> > varNumbers;

    // transforme la formule lue
    write << "c Fichier obtenu par transformation de Tseitin" << endl;
    unsigned varNbr = ClauseTseitin(read, clauses, varNumbers);
    
    // écrit en commentaire l'associations nom/numéro des variables
    write << "c Association noms de variables / numéro de variables :" <<endl;
    for(vector<pair<string,unsigned> >::const_iterator it=varNumbers.begin(); it!=varNumbers.end(); ++it) {
        write << "c\t" << it->first << " :\t" << it->second << endl;
    }
    
    // écrit le problème proprement dit
    write << "p cnf " << varNbr << ' ' << clauses.size() << endl;
    for(vector<clause>::const_iterator it=clauses.begin(); it != clauses.end(); ++it) {
        for(clause::const_iterator lit = it->begin(); lit != it->end(); ++lit) {
            write << (lit->second ? ' ' : '-') << lit->first << ' ';
        }
        write << " 0 " << endl;
    }
    
    // renvoie l'association nom/numéro des variables
    return varNumbers;
}



string executeProg(const char* call)
{
    FILE* sortieProg = popen(call, "r");
    // lit la sortie du programme
    string out;
    char buf[128];
    while (!feof(sortieProg))
    {
        if (fgets(buf, 128, sortieProg) != NULL)
            out += (char*)buf;
    }
    pclose(sortieProg);
    return out;
}
