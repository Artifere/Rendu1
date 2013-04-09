#!/bin/bash
for i in tests-60-288-3-1000/*.cnf
do
  $* < $i
done
exit 0