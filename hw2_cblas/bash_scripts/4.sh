#!/bin/bash
# Check if file "Linux" exists in present directory. If yes, print message "course". If no, print message "very easy" and create the "Linux" file with text "course is easy".

FILE="Linux"
if [ -f $FILE ]; then
	echo "course"
else
	echo "very easy"
	echo "course is easy" > $FILE
fi