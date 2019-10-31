#pragma once
#include "parser.hpp"
#include "expression.hpp"
#include "generator/generator.hpp"
#include <tuple>
#include <vector>
using std::vector;
using std::pair;

class Lexer
{
public:
    Lexer(Generator &generator);
    void parse(Parser &parser);

    inline string get_project_name() const { return project_name == "" ? "TinyLex" : project_name; }

private:
    void parse_statement(Parser &parser, Generator &generator);
    Generator &generator;
    string project_name;

};
