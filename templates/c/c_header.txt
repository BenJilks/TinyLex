
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#ifndef LEXER_H
#define LEXER_H

typedef struct _Token
{
    int data_index;
    int len;
    char type;

    int line_index;
    int line_no, column_no;
    const char *type_name;
} Token;

typedef struct _LexerStream
{
    FILE *file;
    int index, line_index;
    int line, column;
    int eof_flag;

    char *cache;
    Token look;
} LexerStream;

LexerStream lexer_stream_open(const char *file_path);
void lexer_stream_close(LexerStream *stream);
char lexer_read_char(LexerStream *lex, int index);
char *lexer_read_buffer(LexerStream *lex, int index, int len);
void lexer_error(LexerStream *lex, Token token, const char *error);

#endif // LEXER_H

#define TINYLEX_IMPLEMENT
#ifdef TINYLEX_IMPLEMENT

#define CACHE_SIZE      1024
#define RED     "\033[1;31m"
#define ORANGE  "\033[01;33m"
#define WHITE   "\033[37m"
#define RESET   "\033[0m"

LexerStream lexer_stream_open(
    const char *file_path)
{
    LexerStream stream;
    stream.file = fopen(file_path, "rb");
    if (!stream.file)
    {
        printf(RED "Error: could not open file '%s'\n", 
            file_path);
        
        stream.eof_flag = 1;
        stream.cache = NULL;
        return stream;
    }

    stream.eof_flag = 0;
    stream.index = 0;
    stream.line_index = 0;
    stream.column = 0;
    stream.line = 0;
    stream.cache = (char*)malloc(CACHE_SIZE);

    return stream;
}

void lexer_stream_close(
    LexerStream *stream)
{
    if (stream->file)
        fclose(stream->file);
    
    if (stream->cache)
        free(stream->cache);
}

char lexer_read_char(
    LexerStream *lex, 
    int index)
{
    fseek(lex->file, index, SEEK_SET);
    return fgetc(lex->file);
}

char *lexer_read_buffer(
    LexerStream *lex,
    int index, 
    int len)
{
    fseek(lex->file, index, SEEK_SET);
    fread(lex->cache, sizeof(char), len + 1, lex->file);
    return lex->cache;
}

char *lexer_read_line(
    LexerStream *lex, 
    int index, 
    int *len)
{
    *len = 0;
    while (lexer_read_char(lex, index + 1 + *len) != '\n')
        *len += 1;
    
    return lexer_read_buffer(lex, index + 1, *len);
}

void lexer_error(
    LexerStream *lex,
    Token token,
    const char *error)
{
    int len, i;
    char *line;

    printf(RED "Error(%i:%i): %s\n", 
        token.line_no + 1, token.column_no,
        error);

    line = lexer_read_line(lex, token.line_index, &len);
	printf("\t" WHITE);
    fwrite(line, sizeof(char), len, stdout);

    printf("\n\t");
    for (i = 0; i < token.column_no; i++)
        printf(" ");

    printf(ORANGE);
    for (i = 0; i < token.len; i++)
        printf("~");
    printf("\n" RESET);
}

#endif
