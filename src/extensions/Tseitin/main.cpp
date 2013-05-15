#include <iostream>
#include "ExprTree.hh"

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




vector<bool> readAssignation(istream& read, unsigned nbrVars) 
{
    //cout << "nombre de variables : " << nbrVars << endl;
    vector<bool> assign(200000);
    string str;
    char c;
    
    // skip initials comments
    while ((read >> ws) && (! read.eof()) && read.peek() == 'c')
    {
        getline(read, str); // skip the line
        //cout << "skip the line " << str << endl;
    }
    
    read >> ws >> c >> str;
    if ((c != 's') || (str != "SATISFIABLE")) {
        //cout << "quit : unkown out " << c << '|' << str << endl;
        assign.clear();
        return assign;
    }

    while (! read.eof())
    {
        read >> ws >> c >> ws;
        while (c == 'c') {
            getline(read,str);
            read >> ws >> c;
            //cout << "skip the line " << str << endl;
        }
        if (c != 'v') {
            assign.clear();
            return assign;
        }
        //cout << "début de lecture de variable :" << (char)read.peek() << ", " << read.eof() << endl;
        int var = 0;
        //if(! (read >> var)) {
        //    cout << "ecture de int impossible. prochain caractère : " <<  (char)read.peek() < endl;
        //}
        read >> var >> ws;
        //cout << "lecture variable : " << var << endl;
        if (var < 0)
            assign.at(-var) = false;
        else
            assign.at(var) = true;
    }
    return assign;
}



int main(int argc, char * argv[])
{
    if (argc == 3 && string(argv[1]) == "-r")
    {
        //cerr << "c Écriture du problème cnf dans " << argv[2] << endl;
        fstream write(argv[2], ios_base::out | ios_base::trunc);
        const vector<pair<string,unsigned> > assoc(writeProbleme(cin, write));

        // appelle le solver
        string callee("../.././release ");
        callee += argv[2];
        FILE* sortieSolver = popen(callee.c_str(), "r");

        // lit la sortie du programme
        string out;
        char* buf = new char[128];
        while (!feof(sortieSolver))
        {
            if (fgets(buf, 128, sortieSolver) != NULL)
                out += buf;
        }
        pclose(sortieSolver);
        
        // parse la sortie du solver pour récupérer l'assignation
        istringstream outStream(out, ios_base::in);
        //cout << "sortie du solver :" << out << endl << endl;
        
        vector<bool> assign(readAssignation(outStream, assoc.size()));
        
        // affiche l'assignation
        if (assign.empty()) {
            cout << "s UNSATISFIABLE" << endl;
        } else {
            cout << "s SATISFIABLE" << endl;
            for(vector<pair<string,unsigned> >::const_iterator it=assoc.begin(); it!=assoc.end(); ++it) {
                bool val = assign[it->second];
                cout << "v " << (val ? ' ' : '-') << it->first << endl;
            }
        }
    } else {
        writeProbleme(cin,cout);
    }
    return 0;
}




