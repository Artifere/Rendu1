#!/bin/sh

for i in *.cnf
do
  echo $i
  ./exe < $i | grep "s SAT"
done

