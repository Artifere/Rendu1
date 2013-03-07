#!/bin/sh

for i in *.cnf
do
  ./bench < $i
done

