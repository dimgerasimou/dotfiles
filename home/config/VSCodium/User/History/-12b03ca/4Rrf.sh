#!/bin/sh

recursions=10




for i in $(seq 0 $((recursions - 1)));
do
    echo $i
done