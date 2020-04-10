#!/usr/bin/bash

tinylex *.tinylex -l c -o lexer.h > /dev/null
gcc -I./ ../test.c -o test
./test > got.txt

[[ $(diff got.txt expected.txt) ]] &&
	printf "failed" ||
	printf "passed"

rm lexer.h test got.txt
