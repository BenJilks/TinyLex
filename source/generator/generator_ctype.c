#include <stdio.h>
#include "expression.h"
#include "lexer.h"

static void gen_expression(
    FILE *output,
    ExpressionTable *exp)
{
    int i, j;

    fprintf(output, "\n\tstatic int %s_end_states[] = { ", exp->name);
    for (i = 0; i < exp->ending_states.count; i++)
        fprintf(output, "%i, ", exp->ending_states.states[i]);
    fprintf(output, " };\n");
    fprintf(output, "\tstatic char %s[] = \n\t{\n", exp->name);

    for (i = 0; i < exp->state_count; i++)
    {
        fprintf(output, "\t\t");
        for (j = 0; j < 128; j++)
            fprintf(output, "%i, ", exp->table[i * 128 + j]);
        fprintf(output, "\n");
    }
    fprintf(output, "\t};\n");
}

#define FOR_EACH_TYPE(name, ...) \
{ \
    int i; \
    ExpressionTable *exp; \
     \
    fprintf(output, "\tstatic %s = { ", name); \
    for (i = 0; i < lex->table_count; i++) \
    { \
        exp = &lex->tables[i]; \
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
    fprintf(output, "\n\t#define STATE_COUNT %i\n", lex->table_count);
    FOR_EACH_TYPE("char *type_table[]", "%s", exp->name);
    FOR_EACH_TYPE("int *type_end_states[]", "%s_end_states", exp->name);
    FOR_EACH_TYPE("int type_end_state_count[]", "%i", exp->ending_states.count);
    FOR_EACH_TYPE("const char *type_names[]", "\"%s\"", exp->name);
    FOR_EACH_TYPE("int states[]", "0");
    FOR_EACH_TYPE("int last_states[]", "0");
}
