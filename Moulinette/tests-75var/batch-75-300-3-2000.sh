#!/bin/bash
for i in tests-75-300-3-2000/*.cnf
do
  $* < $i
done
exit 0