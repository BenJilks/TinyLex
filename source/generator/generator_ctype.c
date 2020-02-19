#include <stdio.h>
#include "expression.h"
#include "lexer.h"

#define type_name(type) \
    sizeof(type) == 1 ? "char" : \
    (sizeof(type) == 2 ? "short" : \
    (sizeof(type) == 4 ? "int" :  \
    (sizeof(type) == 8 ? "long long" : \
    "unkown")))

static void gen_table(
    FILE *output,
    Lexer *lex)
{
    int i, j;

    fprintf(output, "\t#define NONE (1 << (sizeof(%s) * 8)) - 1\n", type_name(STATE_SIZE));
    fprintf(output, "\n\tstatic char end_states[] = { ");
    for (i = 0; i < lex->table_size; i++)
        fprintf(output, "%i, ", lex->end_states[i]);
    fprintf(output, " };\n");
    fprintf(output, "\tstatic unsigned %s table[] = \n\t{\n", type_name(STATE_SIZE));

    for (i = 0; i < lex->table_size; i++)
    {
        fprintf(output, "\t\t");
        for (j = 0; j < CHAR_COUNT; j++)
            fprintf(output, "%i, ", lex->table[i * CHAR_COUNT + j]);
        fprintf(output, "\n");
    }
    fprintf(output, "\t};\n");
}

#define FOR_EACH_TYPE(name, ...) \
{ \
    int i; \
    Rule *exp; \
     \
    fprintf(output, "\tstatic %s = { ", name); \
    for (i = 0; i < lex->rule_count; i++) \
    { \
        exp = &lex->rules[i]; \
        fprintf(output, __VA_ARGS__); \
        fprintf(output, ", "); \
    } \
    fprintf(output, " };\n"); \
}

static void gen_type_table(
    FILE *output,
    Lexer *lex)
{
    // Type tables
    fprintf(output, "\n\t#define STATE_COUNT %i\n", lex->table_size);
    fprintf(output, "\n\t#define TOKEN_COUNT %i\n", lex->rule_count);
    FOR_EACH_TYPE("const char *type_names[]", "\"%s\"", exp->name);
}
