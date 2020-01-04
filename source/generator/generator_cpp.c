#include "generator.h"
#include "generator_ctype.c"
#include "cpp_header.hpp"
#include "cpp_implement.hpp"
#include "cpp_tinylib.hpp"

static void gen_token_type(
    FILE *output,
    Lexer *lex)
{
    int i;

    // Token struct
    fprintf(output, "\n\tstruct Token\n\t{\n");
    fprintf(output, "\t\tenum TokenType\n\t\t{\n");
    for (i = 0; i < lex->table_count; i++)
        fprintf(output, "\t\t\t%s,\n", lex->tables[i].name);
    fprintf(output, "\t\t};");

    fprintf(output, "\n\t\tstatic const int count = %i;\n", lex->table_count);
    fprintf(output, "\t\tint data, length;\n");

    fprintf(output, "\n\t\tconst char *type_name;\n");
    fprintf(output, "\t\tint line, column;\n");
    fprintf(output, "\t\tTokenType type;\n");
    fprintf(output, "\t};");
}

void generate_cpp(
    FILE *output, 
    Lexer *lex)
{
    int i;

    fputs(template_cpp_tinylib, output);
    fprintf(output, "#ifndef TINY_LEX_HPP\n");
    fprintf(output, "#define TINY_LEX_HPP\n\n");

    fprintf(output, "#include <string>\n");
    fprintf(output, "#include <iostream>\n");
    fprintf(output, "#include <fstream>\n");
    fprintf(output, "#include <vector>\n");
    fprintf(output, "#include <memory.h>\n");
    fprintf(output, "using std::string;\n");
    fprintf(output, "using std::vector;\n");

    fprintf(output, "\nnamespace %s\n{\n", lex->project_name);
    gen_token_type(output, lex);
    fputs(template_cpp_header, output);
    fprintf(output, "\n}\n");

    fprintf(output, "#endif\n");
    fprintf(output, "\n#ifdef TINYLEX_IMPLEMENT\n");
    fprintf(output, "#ifndef TINYLEX_HAS_IMPLEMENT\n");
    fprintf(output, "#define TINYLEX_HAS_IMPLEMENT\n\n");
    fprintf(output, "\nnamespace %s\n{\n", lex->project_name);
    for (i = 0; i < lex->table_count; i++)
        gen_expression(output, &lex->tables[i]);

    gen_type_table(output, lex);
    fputs(template_cpp_implement, output);
    fprintf(output, "\n}\n");

    fprintf(output, "\n#endif\n");
    fprintf(output, "#endif\n");
}
