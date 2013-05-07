#include <iostream>
#include "Tseitin.hh"

#include <iostream>
#include <map>
#include <string>


using namespace std;

int main() {
    Parser parse(cin);
    vector<clause> clauses;
    unsigned varNbr = ClauseTseitin(parse, clauses);
    
    cout << "c Fichier obtenu par transformation de Tseitin" << endl;
    
    cout << "c Association noms de variables / numéro de variables :" <<endl;
    for(map<string,unsigned>::const_iterator it=parse.vars().begin(); it!=parse.vars().end(); ++it) {
        cout << it->first << " :\t" << it->second << endl;
    }
    cout << "c \n";
    
    cout << "p cnf " << varNbr << ' ' << clauses.size() << endl;
    for(vector<clause>::const_iterator it=clauses.begin(); it != clauses.end(); ++it) {
        for(clause::const_iterator lit = it->begin(); lit != it->end(); ++lit) {
            cout << (lit->second ? ' ' : '-') << lit->first << ' ';
        }
        cout << " 0 " << endl;
    }
    return 0;
}




