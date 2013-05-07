#include <iostream>
#include "Tseitin.hh"

#include <iostream>
#include <map>
#include <string>
#include <utility>


using namespace std;

int main() {
    vector<clause> clauses;
    std::vector<pair<std::string,unsigned> > varNumbers;

    cout << "c Fichier obtenu par transformation de Tseitin" << endl;
    unsigned varNbr = ClauseTseitin(cin, clauses, varNumbers);
    
    cout << "c Association noms de variables / numéro de variables :" <<endl;
    for(vector<pair<string,unsigned> >::const_iterator it=varNumbers.begin(); it!=varNumbers.end(); ++it) {
        cout << "c\t" << it->first << " :\t" << it->second << endl;
    }
    
    cout << "p cnf " << varNbr << ' ' << clauses.size() << endl;
    for(vector<clause>::const_iterator it=clauses.begin(); it != clauses.end(); ++it) {
        for(clause::const_iterator lit = it->begin(); lit != it->end(); ++lit) {
            cout << (lit->second ? ' ' : '-') << lit->first << ' ';
        }
        cout << " 0 " << endl;
    }
    return 0;
}




