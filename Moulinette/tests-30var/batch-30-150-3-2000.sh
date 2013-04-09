#!/bin/bash
for i in tests-30-150-3-2000/*.cnf
do
  $* < $i
done
exit 0