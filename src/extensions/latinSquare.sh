#!/bin/bash

k=$1
./latinSquareTransform $k | Tseitin/./tseitin -r foo.out > result.txt
./latinSquarePrint $k < result.txt
#rm foo.out
#rm result.txt

