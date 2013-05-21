#ifndef DEBUG_HH
#define DEBUG_HH

#include <ostream>
#include <vector>
#include <iomanip>



#define DEBUG(n) if (VERBOSE < n) { } else std::cout << "c "


class Variable;
std::ostream& operator<< (std::ostream& out, const Variable& var);


class WatchedClause;
std::ostream& operator<< (std::ostream& out, const WatchedClause* c);

class SmartClause;
std::ostream& operator<< (std::ostream& out, const SmartClause* c);


class Literal;
std::ostream& operator<< (std::ostream& out, const Literal& v);

class SatProblem;
std::ostream& operator<< (std::ostream& out, const SatProblem& sp);


std::ostream& operator<< (std::ostream& out, const std::vector<Literal>& v);




#endif //DEBUG_HH
