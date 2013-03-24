#ifndef CLAUSE_HH
#define CLAUSE_HH

#include "Literal.hh"


#if VERBOSE > 1
#define CONSTR_ARGS(list)  std::vector<Literal>& list, const unsigned int number
#define INIT_FOR_VERBOSE() _number(number),
#define CALL_CONSTR(list)  list, number
#include <iostream>
#else
#define CONSTR_ARGS(list)  std::vector<Literal>& list
#define INIT_FOR_VERBOSE() 
#define CALL_CONSTR(list)   list
#endif


#include "BasicClause.hh"
#include "ConstAssignClause.hh"
#include "OneWatchedClause.hh"
#include "WatchedClause.hh"
#include "SmartClause.hh"
#include "SmartWatchedClause.hh"


#endif //CLAUSE_HH
