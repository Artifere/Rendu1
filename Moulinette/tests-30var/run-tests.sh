#!/bin/bash

# preparation du fichier comparaison.dat: on l'enleve s'il existe, 
# on ecrit l'en-tete des colonnes
rm -f comparaison.dat
echo "argument WatchedSansTri SmartSansTri Watched Smart WatchedRandom SmartRandom SmartMOMSv1 WatchedMOMSv1 minisat" >> comparaison.dat



idTest=0
for script in batch*.sh; do

  echo "je suis en train de traiter l'entree" $script
  /usr/bin/time -f'%U' -o /tmp/toto.txt ./$script ../Executables/benchWatchedSansTri
  TEMPS1=`cat /tmp/toto.txt`
  
  /usr/bin/time -f'%U' -o /tmp/toto.txt ./$script ../Executables/benchSmartSansTri
  TEMPS2=`cat /tmp/toto.txt`
 
  /usr/bin/time -f'%U' -o /tmp/toto.txt ./$script ../Executables/benchWatched
  TEMPS3=`cat /tmp/toto.txt`

  /usr/bin/time -f'%U' -o /tmp/toto.txt ./$script ../Executables/benchSmart
  TEMPS4=`cat /tmp/toto.txt`

  /usr/bin/time -f'%U' -o /tmp/toto.txt ./$script ../Executables/benchWatchedRandom
  TEMPS5=`cat /tmp/toto.txt`

  /usr/bin/time -f'%U' -o /tmp/toto.txt ./$script ../Executables/benchSmartRandom
  TEMPS6=`cat /tmp/toto.txt`

  /usr/bin/time -f'%U' -o /tmp/toto.txt ./$script ../Executables/benchMOMSv1
  TEMPS7=`cat /tmp/toto.txt`

  /usr/bin/time -f'%U' -o /tmp/toto.txt ./$script ../Executables/benchMOMSv1Watched
  TEMPS8=`cat /tmp/toto.txt`
  
  /usr/bin/time -f'%U' -o /tmp/toto.txt ./$script minisat -verb=0
  TEMPS9=`cat /tmp/toto.txt`



  echo $idTest $TEMPS1 $TEMPS2 $TEMPS3 $TEMPS4 $TEMPS5 $TEMPS6 $TEMPS7 $TEMPS8 $TEMPS9 >> ./comparaison.dat 
  idTest=$(($idTest+1))


done