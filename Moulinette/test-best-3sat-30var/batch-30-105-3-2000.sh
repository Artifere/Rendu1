#!/bin/bash
for i in tests-30-105-3-2000/*.cnf
do
  $* < $i
done
exit 0