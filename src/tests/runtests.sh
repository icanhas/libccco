#!/bin/sh
echo
if [ "$1" = "" ] || [ "$2" = "" ]; then
	echo usage: $0 [ repetitions ] [ progs ... ]
	exit 1
fi

reps=$1
fail=0
i=0
while [ $i -lt $# ]; do
	shift
	j=0
	while [ $j -lt $reps ]; do
		echo $1 rep $j
		$1
		if [ $? = 0 ]; then
			echo $1 passed
		else
			echo $1 failed
			fail=1
		fi
		echo
		j=$((j+1))
	done
	i=$((i+1))
done

if [ $fail = 0 ]; then
	echo All tests passed.
else
	echo Tests failed!
fi
