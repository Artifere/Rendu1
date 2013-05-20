#! /bin/bash

make clean ;
for clause in "Smart" "Watched"
do (
    # heuristiques sans tri
    for heur in "BASIC"
    do (
        newName=bench${clause}${heur}AvecTri;
        echo -e "\n***** Génération de $newName *****" ;
        make CLAUSE=${clause}Clause CHOOSE=$heur VERBOSE=0 INIT_SORT=1 ;
        mv release $newName ;
        cp $newName ../Moulinette/Executables ;
        cp $newName ../bin/ ;
        rm $newName ;
        make clean ;
    ) done ;
    # heuristiques sans tri
    for heur in "BASIC" "RAND" "DLIS" "MOMS"
    do (
        newName=bench${clause}${heur};
        echo -e "\n***** Génération de $newName *****" ;
        make CLAUSE=${clause}Clause CHOOSE=$heur VERBOSE=0 INIT_SORT=0 ;
        mv release $newName ;
        cp $newName ../Moulinette/Executables ;
        cp $newName ../bin/ ;
        rm $newName ;
        make clean ;
    ) done ;
) done ;

