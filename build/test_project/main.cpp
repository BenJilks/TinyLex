#define TINYLEX_IMPLEMENT
#include "tinylex.hpp"
#include <iostream>

int main()
{
    TinyScript::Lexer lexer("test.txt");
    while (!lexer.is_eof())
    {
        TinyScript::Token token = lexer.next();
        std::cout << token.data << " - " << token.type_name << std::endl;
    }
    return 0;
}
