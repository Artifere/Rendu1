#!/bin/bash

file=$1
./sudokuTransform $file > foo.out
../Tseitin/tseitin -r foo.cnf < foo.out > result.txt
./sudokuPrint < result.txt
rm foo.cnf
rm foo.out
rm result.txt

