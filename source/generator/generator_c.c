#include "generator.h"
#include "generator_ctype.c"
#include "c_header.h"
#include "c_implement.h"
#include <stdlib.h>

static void generate_implement(
    FILE *output,
    Lexer *lex,
    char *title_upper,
    char *title_lower)
{
    int i;

    fprintf(output, "\n#ifdef %s_IMPLEMENT\n", title_upper);
    fprintf(output, "\n#define LEXER_NAME %s\n", title_lower);
    fprintf(output, "#define INIT_NAME %s_init\n", title_lower);
    fprintf(output, "#define NEXT_MATCH_NAME %s_next_match\n", title_lower);
    fprintf(output, "#define NEXT_NAME %s_next\n", title_lower);
    fprintf(output, "#define MATCH_NAME %s_match\n", title_lower);

    for (i = 0; i < lex->table_count; i++)
        gen_expression(output, &lex->tables[i]);
    gen_type_table(output, lex);
    fputs(template_c_implement, output);

    fprintf(output, "\n#endif\n");
}

static void generate_type_definitions(
    FILE *output, 
    Lexer *lex)
{
    int i;

    fprintf(output, "\ntypedef enum _%sTokenType\n", lex->project_name);
    fprintf(output, "{\n");
    for (i = 0; i < lex->table_count; i++)
    {
        fprintf(output, "\t%s_%s%s,\n", 
            lex->project_name, 
            lex->tables[i].name,
            i == 0 ? " = 0" : "");
    }
    fprintf(output, "} %sTokenType;\n", lex->project_name);
}

void generate_c(
    FILE *output, 
    Lexer *lex)
{
    char *title_upper;
    char *title_lower;

    title_upper = to_upper(lex->project_name);
    title_lower = to_lower(lex->project_name);

    fputs(template_c_header, output);
    fprintf(output, "\n#ifndef %s_H\n", title_upper);
    fprintf(output, "#define %s_H\n", title_upper);
    generate_type_definitions(output, lex);

    fprintf(output, "\nvoid %s_init(LexerStream *lex);\n", title_lower);
    fprintf(output, "Token %s_next(LexerStream *lex);\n", title_lower);
    fprintf(output, "Token %s_match(LexerStream *lex, char type, const char *name);\n", 
        title_lower);

    fprintf(output, "\n#endif\n");
    generate_implement(output, lex, 
        title_upper, title_lower);

    free(title_upper);
    free(title_lower);
}
