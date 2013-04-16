#!/bin/sh

for i in *.cnf
do
  echo $i
  ./exe $i > $i$".out"
  cat $i $i$".out" | ./verif
  rm $i$".out"
done

