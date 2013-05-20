#!/bin/bash

./latinSquareTransform $* > foo.out
../Tseitin/tseitin -r foo.cnf < foo.out > result.txt
./latinSquarePrint $1 < result.txt

rm foo.out
rm foo.cnf
rm result.txt

