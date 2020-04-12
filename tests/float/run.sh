#!/usr/bin/bash

function run_lang
{
	tinylex *.tinylex -l $1 -o lexer.h > /dev/null
	$2 -I./ ../$3 -o test
	./test > got.txt

	diff got.txt expected.txt
	rm lexer.h test got.txt
}

run_lang c gcc test.c
run_lang cpp g++ test.cpp
