#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

#define CHAR_COUNT  128
#define STATE_SIZE  char
#define NONE (1 << (sizeof(STATE_SIZE) * 8)) - 1

typedef struct _Stream
{
    FILE *file;
    char *line_buffer;
    int line_length;
    int line_no, column_no;

    char look;
    char eof_flag;
    char error_flag;
} Stream;

Stream parser_open_stream(const char *file_path);
void parser_close_stream(Stream *stream);

void parser_match(Stream *stream, char c);
void parser_skip_white_space(Stream *stream);
char parser_next_char(Stream *stream);
void parser_next_word(Stream *stream, char *buffer);

#endif // PARSER_H
