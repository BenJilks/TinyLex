#include "generator.h"
#include "generator_ctype.c"
#include <stdlib.h>

void generate_c(
    FILE *output, 
    Lexer *lex)
{
    int i;
    char *title_upper;
    char *title_lower;

    title_upper = to_upper(lex->project_name);
    title_lower = to_lower(lex->project_name);

    write_file(output, "c/c_header.h");
    fprintf(output, "\n#ifndef %s_H\n", title_upper);
    fprintf(output, "#define %s_H\n", title_upper);

    fprintf(output, "\nvoid %s_init(LexerStream *lex);\n", title_lower);
    fprintf(output, "Token %s_next(LexerStream *lex);\n", title_lower);
    fprintf(output, "Token %s_match(LexerStream *lex, int type, const char *name);\n", 
        title_lower);

    fprintf(output, "\n#endif\n");

    fprintf(output, "\n#ifdef %s_IMPLEMENT\n", title_upper);
    fprintf(output, "\n#define LEXER_NAME %s\n", title_upper);

    for (i = 0; i < lex->table_count; i++)
        gen_expression(output, &lex->tables[i]);
    gen_type_table(output, lex);
    write_file(output, "c/c_implement.txt");

    fprintf(output, "\n#endif\n");

    free(title_upper);
    free(title_lower);
}
