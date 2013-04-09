#!/bin/bash
for i in tests-40-160-3-500/*.cnf
do
  $* < $i
done
exit 0