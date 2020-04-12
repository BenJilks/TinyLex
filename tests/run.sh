#!/usr/bin/bash

TEST_COUNT=0
function do_test
{
	cd $1
	printf "["
	[[ $(./run.sh) ]] && 
		printf "failed" || 
		printf "passed"
	printf "] $1\n"
	cd ..
	TEST_COUNT=$(( $TEST_COUNT+1 ))
}

do_test keywords
do_test optional
do_test any
do_test one_or_more
do_test symbols
do_test range
do_test range_not
do_test range_optional
do_test range_special
do_test float
echo "Ran $TEST_COUNT test(s)"

