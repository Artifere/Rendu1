#!/bin/bash

k=$1
file=$2
./coloringTransform $k < $file > fooS.out
../Tseitin/tseitin -r foo.out < fooS.out > result.txt
./coloringPrint $k $file result.txt > colored.dot
res=$?
if [ $res -eq 0 ]
    then
    dot -Tpdf colored.dot -o colored.pdf
else
    echo -e "Le graphe n'est pas ${k}-coloriable."
fi

rm foo.out
rm fooS.out
rm colored.dot
rm result.txt
