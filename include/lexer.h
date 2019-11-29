#ifndef LEXER_H
#define LEXER_H

#include "parser.h"
#include "expression.h"

typedef struct _Lexer
{
    char project_name[80];

    ExpressionTable *tables;
    int table_count;
    int table_buffer_size;
} Lexer;

Lexer lexer_create();
void lexer_parse(Stream *stream, Lexer *lex);
void lexer_free(Lexer lexer);

#endif // LEXER_H
