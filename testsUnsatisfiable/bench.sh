#!/bin/sh

for i in *.cnf
do
  ./exe  $i > /dev/null
  echo $i
done

