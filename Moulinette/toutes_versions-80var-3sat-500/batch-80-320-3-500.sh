#!/bin/bash
for i in tests-80-320-3-500/*.cnf
do
  $* < $i
done
exit 0