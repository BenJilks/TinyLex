#include "generator.h"

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

static void gen_token_type(
    FILE *output,
    Lexer *lex)
{
    int i;

    // Token struct
    fprintf(output, "\n\tstruct Token\n\t{\n");
    fprintf(output, "\t\tenum TokenType\n\t\t{\n");
    for (i = 0; i < lex->table_count; i++)
        fprintf(output, "\t\t\t%s,\n", lex->tables[i]);
    fprintf(output, "\t\t};");

    fprintf(output, "\n\t\tstatic const int count = %i;\n", lex->table_count);
    fprintf(output, "\t\tint data, length;\n");

    fprintf(output, "\n\t\tconst char *type_name;\n");
    fprintf(output, "\t\tint line, column;\n");
    fprintf(output, "\t\tTokenType type;\n");
    fprintf(output, "\t};");
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
    FOR_EACH_TYPE("char *type_table", "%s", exp->name);
    FOR_EACH_TYPE("int *type_end_states[]", "%s_end_states", exp->name);
    FOR_EACH_TYPE("int type_end_state_count[]", "%i", exp->ending_states.count);
    FOR_EACH_TYPE("const char *type_names[]", "\"%s\"", exp->name);
    FOR_EACH_TYPE("int states[]", "0");
    FOR_EACH_TYPE("int last_states[]", "0");
}

void generate_cpp(
    FILE *output, 
    Lexer *lex)
{
    int i;

    write_file(output, "cpp_tinylib.hpp");
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
    write_file(output, "cpp_header.txt");
    fprintf(output, "\n}\n");

    fprintf(output, "#endif\n");
    fprintf(output, "\n#ifdef TINYLEX_IMPLEMENT\n");
    fprintf(output, "#ifndef TINYLEX_HAS_IMPLEMENT\n");
    fprintf(output, "#define TINYLEX_HAS_IMPLEMENT\n\n");
    fprintf(output, "\nnamespace %s\n{\n", lex->project_name);
    for (i = 0; i < lex->table_count; i++)
        gen_expression(output, &lex->tables[i]);

    gen_type_table(output, lex);
    write_file(output, "cpp_implement.txt");
    fprintf(output, "\n}\n");

    fprintf(output, "\n#endif\n");
    fprintf(output, "#endif\n");
}
