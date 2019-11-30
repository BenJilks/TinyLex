
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#ifndef LEXER_H
#define LEXER_H

typedef struct _Token
{
    int data;
    int len;
    char type;

    const char *type_name;
} Token;

typedef struct _LexerStream
{
    FILE *file;
    int index;
    int column, line;
    int eof_flag;

    char *cache;
    Token look;
} LexerStream;

LexerStream lexer_stream_open(const char *file_path);
void lexer_stream_close(LexerStream *stream);
char lexer_read_char(LexerStream *lex, int index);
char *lexer_read_buffer(LexerStream *lex, int index, int len);

#endif // LEXER_H

#ifdef TINYLEX_IMPLEMENT

#define CACHE_SIZE      80

LexerStream lexer_stream_open(
    const char *file_path)
{
    LexerStream stream;
    stream.file = fopen(file_path, "rb");
    stream.index = 0;
    stream.column = 0;
    stream.line = 0;
    stream.cache = (char*)malloc(CACHE_SIZE);

    return stream;
}

void lexer_stream_close(
    LexerStream *stream)
{
    fclose(stream->file);
    free(stream->cache);
}

char lexer_read_char(
    LexerStream *lex, 
    int index)
{
    fseek(lex->file, 0L, index);
    return fgetc(lex->file);
}

char *lexer_read_buffer(
    LexerStream *lex,
    int index, 
    int len)
{
    fseek(lex->file, 0L, index);
    fread(lex->cache, sizeof(char), len + 1, lex->file);
    return lex->cache;
}

#endif
