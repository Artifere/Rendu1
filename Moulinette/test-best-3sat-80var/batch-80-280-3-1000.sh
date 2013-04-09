#!/bin/bash
for i in tests-80-280-3-1000/*.cnf
do
  $* < $i
done
exit 0