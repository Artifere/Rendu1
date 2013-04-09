#!/bin/bash
for i in tests-50-400-3-2000/*.cnf
do
  $* < $i
done
exit 0