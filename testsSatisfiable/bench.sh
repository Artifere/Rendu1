#!/bin/sh

for i in *.cnf
do
  minisat -verb=0 < $i
done

