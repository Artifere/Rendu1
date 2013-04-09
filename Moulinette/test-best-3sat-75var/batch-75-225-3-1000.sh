#!/bin/bash
for i in tests-75-225-3-1000/*.cnf
do
  $* < $i
done
exit 0