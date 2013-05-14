#!/bin/bash

k=$1
file=$2
./coloringTransform $k < $file > fooS.out
Tseitin/./tseitin -r foo.out < fooS.out > result.txt
cat result.txt
./coloringPrint $k $file result.txt > test.out
#rm foo.out
#rm result.txt

