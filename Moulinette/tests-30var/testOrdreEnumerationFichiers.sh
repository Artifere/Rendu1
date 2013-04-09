#!/bin/bash
truc=ls |grep batch |sort -n
for i in $truc
do
  echo $i
  done
ls |grep batch | sort -n
#machin= sort $truc -n
echo $truc
#for i in $machin
#do
#  echo $i
#done
exit 0
