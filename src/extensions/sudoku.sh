#!/bin/bash

file=$1
./sudokuTransform $file > fooS.out
Tseitin/./tseitin -r foo.out < fooS.out > result.txt
./sudokuPrint < result.txt
#rm foo.out
#rm result.txt

