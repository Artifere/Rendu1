#!/bin/bash
for i in tests-75-150-3-2000/*.cnf
do
  $* < $i
done
exit 0