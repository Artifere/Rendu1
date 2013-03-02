#!/bin/sh

for i in *.cnf
do
  echo $i
  ./exe < $i > $i$".out"
  ./verif < $i$".out"
done

