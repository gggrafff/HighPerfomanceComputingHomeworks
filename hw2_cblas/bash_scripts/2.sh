#!/bin/bash
# Initialize the array of 10-20 elements and organize FOR loop printing the elements of array.

array=({0..15})

for value in ${array[*]}
do
	echo $value
done