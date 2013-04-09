#!/bin/bash

# preparation du fichier comparaison.dat: on l'enleve s'il existe, 
# on ecrit l'en-tete des colonnes
rm -f comparaison.dat
echo "argument minisat SmartDLIS WatchedDLIS smartBASICTri" >> comparaison.dat



idTest=0
for script in batch*.sh; do

  echo "je suis en train de traiter l'entree" $script
  /usr/bin/time -f'%U' -o /tmp/toto.txt ./$script minisat -verb=0
  TEMPS1=`cat /tmp/toto.txt`
  
  /usr/bin/time -f'%U' -o /tmp/toto.txt ./$script ../Executables/benchSmartDLIS
  TEMPS2=`cat /tmp/toto.txt`
 
  /usr/bin/time -f'%U' -o /tmp/toto.txt ./$script ../Executables/benchWatchedDLIS
  TEMPS3=`cat /tmp/toto.txt`

  /usr/bin/time -f'%U' -o /tmp/toto.txt ./$script ../Executables/benchSmartBASICAvecTri
  TEMPS4=`cat /tmp/toto.txt`


  echo $idTest $TEMPS1 $TEMPS2 $TEMPS3 $TEMPS4 >> ./comparaison.dat 
  idTest=$(($idTest+1))


done