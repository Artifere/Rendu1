#!/bin/bash
for i in tests-60-240-3-1000/*.cnf
do
  $* < $i
done
exit 0