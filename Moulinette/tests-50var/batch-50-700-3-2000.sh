#!/bin/bash
for i in tests-50-700-3-2000/*.cnf
do
  $* < $i
done
exit 0