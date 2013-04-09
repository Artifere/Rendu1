#!/bin/bash
for i in tests-50-250-3-2000/*.cnf
do
  $* < $i
done
exit 0