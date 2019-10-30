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
    Lexer(Parser &parser, Generator &generator);

private:
    void parse_statement(Parser &parser, Generator &generator);

};
