#!/bin/bash
for i in tests-75-675-3-1000/*.cnf
do
  $* < $i
done
exit 0