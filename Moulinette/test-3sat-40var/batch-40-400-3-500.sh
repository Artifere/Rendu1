#!/bin/bash
for i in tests-40-400-3-500/*.cnf
do
  $* < $i
done
exit 0