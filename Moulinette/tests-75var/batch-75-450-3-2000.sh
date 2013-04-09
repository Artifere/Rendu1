#!/bin/bash
for i in tests-75-450-3-2000/*.cnf
do
  $* < $i
done
exit 0