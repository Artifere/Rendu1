#! /bin/bash

make clean ;
for clause in "SmartClause" "WatchedClause" "SmartWatchedClause"
do (
  newName= bench$clause ;
  echo "Génération de $newName :" ;
  make CLAUSE=$clause VERBOSE=0 ;
  mv release $newName ;
  cp $newName Moulinette/ ;
  cp $newName Moulinette/ ;
  rm $newName ;
  make clean ;
) done ;

