#!/bin/bash

NUMBER_SUBMISSIONS=3
SUBMISSION_VAL=1000
INDEX=2
INDEX1=0

while [ $NUMBER_SUBMISSIONS -gt 0 ]; do
	while [ $INDEX -lt 64 ]; do
		while [ $INDEX1 -lt 3 ]; do	
			qsub -pe ompi $INDEX -v ARG1=$SUBMISSION_VAL submit.sh
			echo numsubs $NUMBER_SUBMISSIONS subval $SUBMISSION_VAL ind $INDEX ind1 $INDEX1
			INDEX1=$((INDEX1+1))
		done
		INDEX=$((INDEX*2))
		INDEX1=0
	done
	INDEX=2
	SUBMISSION_VAL=$((SUBMISSION_VAL/10))
	NUMBER_SUBMISSIONS=$((NUMBER_SUBMISSIONS-1))
done
