#include "generator/generator_c.hpp"

void CGenerator::gen_expression(string name, ExpressionTable *exp)
{
    write_string("\n\tstatic int " + name + "_end_states[] = {");
    for (int state : exp->get_end_states())
        write_string(std::to_string(state) + ", ");
    write_line("};");
    write_line("\tstatic char " + name + "[] = \n\t{");

    char *table = exp->get_data();
    for (int i = 0; i < exp->get_size(); i++)
    {
        write_string("\t\t");
        for (int j = 0; j < 128; j++)
            write_string(std::to_string(table[i * 128 + j]) + ", ");
        write_string("\n");
    }
    write_line("\t};");
}

void CGenerator::gen_token_type()
{
    // Token struct
    write_line("\n\tstruct Token\n\t{");
    write_line("\t\tenum TokenType\n\t\t{");
    for (auto exp : expressions)
        write_line("\t\t\t" + exp.first + ",");
    write_line("\t\t};");

    write_line("\n\t\tstatic const int count = " + std::to_string(expressions.size()) + ";");
    write_line("\t\tint data, length;");

    write_line("\n\t\tconst char *type_name;");
    write_line("\t\tint line, column;");
    write_line("\t\tTokenType type;");
    write_line("\t};");
}

void CGenerator::gen_for_each_type(string name, 
    std::function<string(Expression)> func)
{
    write_string("\tstatic " + name + " = {");
    for (auto exp : expressions)
        write_string(func(exp) + ", ");
    write_line("};");
}

void CGenerator::gen_type_table()
{
    // Type tables
    write_line("\n\t#define STATE_COUNT " + std::to_string(expressions.size()));

    gen_for_each_type("char *type_table[]", [](Expression exp) -> string 
        { return exp.first; });
    
    gen_for_each_type("int *type_end_states[]", [](Expression exp) -> string 
        { return exp.first + "_end_states"; });
    
    gen_for_each_type("int type_end_state_count[]", [](Expression exp) -> string 
        { return std::to_string(exp.second->get_end_states().size()); });

    gen_for_each_type("const char *type_names[]", [](Expression exp) -> string 
        { return "\"" + exp.first + "\""; });

    gen_for_each_type("int states[]", [](Expression exp) -> string { return "0"; });
    gen_for_each_type("int last_states[]", [](Expression exp) -> string { return "0"; });
}

void CGenerator::generate(string project_name)
{
    write_line("#pragma once");
    write_line("#include <string>");
    write_line("#include <iostream>");
    write_line("#include <fstream>");
    write_line("#include <vector>");
    write_line("#include <memory.h>");
    write_line("using std::string;");
    write_line("using std::vector;");

    write_line("\nnamespace " + project_name + "\n{");
    gen_token_type();
    write_file("cpp_header.txt");

    write_line("\n#ifdef TINYLEX_IMPLEMENT");
    for (auto exp : expressions)
    {
        string name = exp.first;
        ExpressionTable *table = exp.second;
        gen_expression(name, table);
    }
    gen_type_table();
    write_file("cpp_implement.txt");
    write_line("\n#endif");

    write_line("\n}");
}
