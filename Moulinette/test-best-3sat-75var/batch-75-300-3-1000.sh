#!/bin/bash
for i in tests-75-300-3-1000/*.cnf
do
  $* < $i
done
exit 0