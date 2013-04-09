#!/bin/bash
for i in tests-80-700-3-500/*.cnf
do
  $* < $i
done
exit 0