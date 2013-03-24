#! /bin/bash

make clean ;
for clause in "SmartClause" "WatchedClause" "SmartWatchedClause"
do (
  echo "Génération de $clause :" ;
  make CLAUSE=$clause VERBOSE=0 ;
  mv release b_$clause
  cp b_$clause testsSatisfiable/ ;
  cp b_$clause testsUnsatisfiable/ ;
#  rm b_$clause
  make clean ;
) done ;

