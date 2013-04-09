#!/bin/bash
for i in tests-40-300-3-500/*.cnf
do
  $* < $i
done
exit 0