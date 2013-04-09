#!/bin/bash
for i in tests-50-240-3-2000/*.cnf
do
  $* < $i
done
exit 0