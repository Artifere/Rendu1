#!/bin/bash

k=$1
file=$2
./coloringTransform $k < $file > fooS.out
Tseitin/./tseitin -r foo.out < fooS.out > result.txt
./coloringPrint $k $file result.txt > test.out
res=$?
if [ $res -eq 0 ]
    then
    dot -Tpdf test.out -o test.pdf
    evince test.pdf&
else
    echo -e "Le graphe n'est pas ${k}-coloriable."
fi
#rm foo.out
#rm result.txt

