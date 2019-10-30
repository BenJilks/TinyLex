#include <stdio.h>
#include "lexer.hpp"
#include "generator/generator_c.hpp"

int main()
{
    CGenerator generator("test_project/tinylex.hpp");
    Parser parser("../test.tinylex");
    Lexer lexer(parser, generator);

    generator.generate();
    return 0;
}
