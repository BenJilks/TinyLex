#ifndef LEXER_H
#define LEXER_H

#include "parser.h"
#include "expression.h"

typedef struct _Lexer
{
    // Project info
    char project_name[80];

    // Compiled tables
    Rule *rules;
    int rule_count;
    int rule_buffer_size;

    // Linked output table
    unsigned STATE_SIZE *table;
    char *end_states;
    int table_size;
} Lexer;

Lexer lexer_create();
void lexer_parse(Stream *stream, Lexer *lex);
void lexer_link(Lexer *lex);
void lexer_free(Lexer lexer);

#endif // LEXER_H
