#!/bin/bash

./additionTransform $1 $2 > foo.out
../Tseitin/tseitin < foo.out > foo.cnf
./additionPrint < result.txt
rm foo.cnf
rm foo.out
rm result.txt

