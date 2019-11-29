#include "lexer.h"
#include "debug.h"
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 80

static void parse_expression_rule(
    Lexer *lex, 
    Stream *stream, 
    char *name)
{
    ExpressionTable table;

    // Parse and set table name
    LOG("   => Parsing rule '%s'\n", name);
    table = expression_parse(stream);
    strcpy(table.name, name);

    // Add rule to lexer
    lex->tables[lex->table_count] = table;
    lex->table_count += 1;

    // If the buffer has been filled, expand it
    if (lex->table_count >= lex->table_buffer_size)
    {
        lex->table_buffer_size += BUFFER_SIZE;
        lex->tables = realloc(lex->tables, 
            sizeof(ExpressionTable) * lex->table_buffer_size);
    }

    parser_skip_white_space(stream);
}

Lexer lexer_create()
{
    Lexer lex;
    lex.tables = malloc(sizeof(ExpressionTable) * BUFFER_SIZE);
    lex.table_count = 0;
    lex.table_buffer_size = BUFFER_SIZE;
    LOG("Created new lexer\n");

    return lex;
}

void lexer_parse(
    Stream *stream,
    Lexer *lex)
{
    char name[80];
    LOG("Started parsing\n");

    parser_skip_white_space(stream);
    while (!stream->eof_flag)
    {
        // Fetch the first word of the statement
        parser_next_word(stream, name);
        parser_skip_white_space(stream);

        // If it's a keyword, do the appropriate action, 
        // otherwise parse an expression rule
        if (!strcmp(name, "project"))
        {
            parser_next_word(stream, lex->project_name);
            parser_skip_white_space(stream);
        }
        else
            parse_expression_rule(lex, stream, name);
    }
}

void lexer_free(Lexer lexer)
{
    free(lexer.tables);
}
