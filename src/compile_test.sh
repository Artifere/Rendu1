#! /bin/bash

make clean ;
for clause in "Smart" "Watched"
do (
    for heur in "BASIC" "RAND" "DLIS"
    do (
        # avec tri
        newName=exe${clause}${heur}AvecTri;
        echo "***** Génération de $newName *****" ;
        make CLAUSE=${clause}Clause CHOOSE=$heur VERBOSE=1 INIT_SORT=1 ;
        mv release $newName ;
        cp $newName ../bin/ ;
        rm $newName ;
        make clean ;
        # sans tri
        newName=exe${clause}${heur};
        echo "***** Génération de $newName *****" ;
        make CLAUSE=${clause}Clause CHOOSE=$heur VERBOSE=1 INIT_SORT=0 ;
        mv release $newName ;
        cp $newName ../bin/ ;
        rm $newName ;
        make clean ;
    ) done ;
) done ;

