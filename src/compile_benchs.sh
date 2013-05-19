#! /bin/bash

make clean ;
for clause in "Smart" "Watched" "SmartWatched" "ConstAssign" "Basic"
do (
    for heur in "BASIC" "RAND" "DLIS"
    do (
        # avec tri
        newName=bench${clause}${heur}AvecTri;
        echo "***** Génération de $newName *****" ;
        make CLAUSE=${clause}Clause CHOOSE=$heur VERBOSE=0 INIT_SORT=1 ;
        mv release $newName ;
        cp $newName ../Moulinette/Executables ;
        cp $newName ../bin/ ;
        rm $newName ;
        make clean ;
        # sans tri
        newName=bench${clause}${heur};
        echo "***** Génération de $newName *****" ;
        make CLAUSE=${clause}Clause CHOOSE=$heur VERBOSE=0 INIT_SORT=0 ;
        mv release $newName ;
        cp $newName ../Moulinette/Executables ;
        cp $newName ../bin/ ;
        rm $newName ;
        make clean ;
    ) done ;
) done ;

