#!/bin/bash

# preparation du fichier comparaison.dat: on l'enleve s'il existe, 
# on ecrit l'en-tete des colonnes
rm -f comparaison.dat
echo "argument V1 V1Watched SmartBASIC SmartBASICTri SmartRAND SmartDLIS WatchedBASICTRI WatchedRAND WatchedDLIS" >> comparaison.dat



idTest=0
for script in batch*.sh; do

  echo "je suis en train de traiter l'entree" $script
  /usr/bin/time -f'%U' -o /tmp/toto.txt ./$script ../Executables/benchConstAssignBASIC
  TEMPS1=`cat /tmp/toto.txt`
  
  /usr/bin/time -f'%U' -o /tmp/toto.txt ./$script ../Executables/benchWatchedBASIC
  TEMPS2=`cat /tmp/toto.txt`
 
  /usr/bin/time -f'%U' -o /tmp/toto.txt ./$script ../Executables/benchSmartBASIC
  TEMPS3=`cat /tmp/toto.txt`

  /usr/bin/time -f'%U' -o /tmp/toto.txt ./$script ../Executables/benchSmartBASICAvecTri
  TEMPS4=`cat /tmp/toto.txt`

  /usr/bin/time -f'%U' -o /tmp/toto.txt ./$script ../Executables/benchSmartRAND
  TEMPS5=`cat /tmp/toto.txt`

  /usr/bin/time -f'%U' -o /tmp/toto.txt ./$script ../Executables/benchSmartDLIS
  TEMPS6=`cat /tmp/toto.txt`

  /usr/bin/time -f'%U' -o /tmp/toto.txt ./$script ../Executables/benchWatchedBASICAvecTri
  TEMPS7=`cat /tmp/toto.txt`

  /usr/bin/time -f'%U' -o /tmp/toto.txt ./$script ../Executables/benchWatchedRAND
  TEMPS8=`cat /tmp/toto.txt`
  
  /usr/bin/time -f'%U' -o /tmp/toto.txt ./$script ../Executables/benchWatchedDLIS
  TEMPS9=`cat /tmp/toto.txt`



  echo $idTest $TEMPS1 $TEMPS2 $TEMPS3 $TEMPS4 $TEMPS5 $TEMPS6 $TEMPS7 $TEMPS8 $TEMPS9 >> ./comparaison.dat 
  idTest=$(($idTest+1))


done