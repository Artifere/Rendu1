#!/bin/bash
for i in tests-75-285-3-1000/*.cnf
do
  $* < $i
done
exit 0