#define TINYLEX_IMPLEMENT
#define TEST_IMPLEMENT
#include "lexer.h"
#include <stdio.h>

int main()
{
	LexerStream lex;
	char *data;
	int data_buffer;

	lex = lexer_stream_open("test.txt");
	test_init(&lex);

	data = malloc(80);
	data_buffer = 80;
	while (!lex.eof_flag)
	{
		Token token;

		token = test_next(&lex);
		if (token.len > data_buffer)
		{
			data_buffer = token.len;
			data = realloc(data, data_buffer);
		}

		lexer_read_string(&lex, token, data);
		printf("%s,%s\n", data, token.type_name);
	}

	lexer_stream_close(&lex);
	free(data);
}

