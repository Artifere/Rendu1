#!/bin/bash
for i in tests-75-412-3-1000/*.cnf
do
  $* < $i
done
exit 0