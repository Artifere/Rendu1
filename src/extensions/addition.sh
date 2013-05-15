#!/bin/bash

./additionTransform $1 $2 | Tseitin/./tseitin -r foo.out > result.txt &&
./additionPrint < result.txt
#rm foo.out
#rm result.txt

