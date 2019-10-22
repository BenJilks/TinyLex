#include <stdio.h>
#include "lexer.hpp"
#include "generator/generator_c.hpp"

int main()
{
    CGenerator generator;
    Parser parser("../test.tinylex");
    Lexer lexer(parser, generator);

    generator.generate();
    return 0;
}
