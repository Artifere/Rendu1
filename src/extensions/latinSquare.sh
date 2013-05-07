#!/bin/bash

k=$1
./latinSquareTransform $k | Tseitin/./tseitin -r foo.out > result.txt
./latinSquarePrint $k < result.txt

