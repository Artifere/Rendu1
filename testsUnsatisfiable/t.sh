#!/bin/sh

for i in exe*
do
    echo $i
    ./test.sh ./$i
done

