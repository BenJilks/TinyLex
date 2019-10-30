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
    write_line("\n\tstruct Token\n\t{");
    write_line("\t\tenum TokenType\n\t\t{");
    for (auto exp : expressions)
        write_line("\t\t\tTOKEN_" + exp.first + ",");
    write_line("\t\t};");

    write_line("\n\t\tstatic const int count = " + std::to_string(expressions.size()) + ";");
    write_line("\t\tstring data;");
    write_line("\t\tTokenType type;");
    write_line("\t};");
}

void CGenerator::gen_type_table()
{
    // Type tables
    write_string("\n\tstatic char *type_table[] = {");
    for (auto exp : expressions)
        write_string(exp.first + ", ");
    write_line("};");

    // Type end states
    write_string("\n\tstatic int *type_end_states[] = {");
    for (auto exp : expressions)
        write_string(exp.first + "_end_states, ");
    write_line("};");

    // Type end state counts
    write_string("\n\tstatic int type_end_state_count[] = {");
    for (auto exp : expressions)
        write_string(std::to_string(exp.second->get_end_states().size()) + ", ");
    write_line("};");
}

void CGenerator::generate()
{
    write_line("#pragma once");
    write_line("#include <string>");
    write_line("#include <iostream>");
    write_line("#include <fstream>");
    write_line("using std::string;");

    write_line("\nnamespace TinyScript\n{");
    gen_token_type();
    write_file("cpp/header.txt");

    write_line("\n#ifdef TINYLEX_IMPLEMENT");
    for (auto exp : expressions)
    {
        string name = exp.first;
        ExpressionTable *table = exp.second;
        gen_expression(name, table);
    }
    gen_type_table();
    write_file("cpp/implement.txt");
    write_line("\n#endif");

    write_line("\n}");
}
