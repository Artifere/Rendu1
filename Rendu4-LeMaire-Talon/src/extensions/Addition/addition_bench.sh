#!/bin/bash

./additionTransform $2 $3 > foo.out

for i in `seq 1 $1`;
do
 ../Tseitin/tseitin -r foo.cnf < foo.out > result.txt
done
./additionPrint < result.txt
rm foo.cnf
rm foo.out
rm result.txt

