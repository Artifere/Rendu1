#!/bin/bash

k=$0
./latinSquareTransform $k | Tseitin/./tseitin -r foo.out > result.txt
./latinSquarePrint $k < result.txt

