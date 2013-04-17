#! /bin/bash

make clean ;
for clause in "Smart"
do (
    for heur in "BASIC" "RAND" "DLIS"
    do (
        # avec tri
        newName=bench${clause}${heur}AvecTriLearning;
        echo "***** Génération de $newName *****" ;
        make CLAUSE=${clause}Clause CHOOSE=$heur VERBOSE=0 INIT_SORT=1 ;
        mv release $newName ;
        cp $newName Moulinette/Executables ;
        rm $newName ;
        make clean ;
        # sans tri
        newName=bench${clause}${heur}Learning;
        echo "***** Génération de $newName *****" ;
        make CLAUSE=${clause}Clause CHOOSE=$heur VERBOSE=0 INIT_SORT=0 ;
        mv release $newName ;
        cp $newName Moulinette/Executables ;
        rm $newName ;
        make clean ;
    ) done ;
) done ;

