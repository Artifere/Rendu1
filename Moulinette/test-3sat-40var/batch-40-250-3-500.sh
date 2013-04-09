#!/bin/bash
for i in tests-40-250-3-500/*.cnf
do
  $* < $i
done
exit 0