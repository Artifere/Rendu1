#include <iostream>
#include "clauses.hh"
#include "buckets.hh"
#include <cmath>
#include <algorithm>
#include "davis.hh"
#include <cstdio>

using namespace std;

int main()
{
    string read;
    char c;

    while (cin.peek() == ' ' || cin.peek() == '\n' || cin.peek() == '\r')
        cin >> c;
    while (cin.peek() == 'c')
        getline(cin, read);

    //If the formula is empty
    if (cin.peek() == EOF)
    {
      cout << "s SATISFIABLE";
      return 0;
    }

    cin >> c >> read;
    if (c != 'p' || read != "cnf")
      cout << "c Arg, entrée invalide" << endl << c << endl << read;


    int nbVar, nbClauses;
    int var, maxVar;
    cin >> nbVar >> nbClauses;

    //posBuckets[ind] contains the liste of clauses which max litteral is ind
    //negBuckets[ind] contains the liste of clauses which max litteral is -ind
    Bucket *posBuckets = new Bucket[nbVar+1];
    Bucket *negBuckets = new Bucket[nbVar+1];


    for (int cl = 0; cl < nbClauses; cl++)
    {
      while (cin.peek() == ' ')
        getchar();
      while (cin.peek() == 'c')
        getline(cin, read);

      //We use a set to store the litterals of a clause when we read it. This enables
      //us not to have mutiple identical litterals
      set<Litt> readSet;


      cin >> var;

      maxVar = var;
      while (var != 0)
      {
        if (abs(var) > abs(maxVar))
          maxVar = var;
        readSet.insert(Litt(abs(var), var > 0));
        cin >> var;
      }


      if (var != 0)
        cout << "c Erreur, pas de 0 terminant" << endl;

      // We transer the clause from the set to a clause object and put it in the right bucket.
      // If the clause contains some x and -x, we skip it.
      set<Litt>::iterator stop = readSet.end();
      //We won't put the "max" litteral in the clause. This litteral is indicated by the bucket the
      //clause is put in.

      if (readSet.size() == 0)
        continue;
      stop--;

      vector<Litt> readVector(readSet.size());

      vector<Litt>::iterator pos = readVector.begin();
      for (set<Litt>::iterator it = readSet.begin(); it != stop; ++pos)
      {
        *pos = *it;
        ++it;
        //x -x
        if (it->id() == pos->id())
          continue;
      }
      readVector.resize(pos-readVector.begin());
      Clause readClause = Clause(readVector);

      if (maxVar > 0)
        posBuckets[maxVar].push(readClause);
      else
        negBuckets[-maxVar].push(readClause);
    }



    if (is_satisfiable(posBuckets, negBuckets, nbVar))
    {
      cout << "s SATISFIABLE" << endl;
      vector<bool> assignation = find_assignation(posBuckets, negBuckets, nbVar);
      for (int var = 1; var <= nbVar; var++)
        cout << "v " << (assignation[var] ? var:(-var)) << endl;
    }

    else
      cout << "s UNSATISFIABLE" << endl;

    delete[] posBuckets;
    delete[] negBuckets;

    return 0;
}
