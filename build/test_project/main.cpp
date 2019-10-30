#define TINYLEX_IMPLEMENT
#include "tinylex.hpp"
#include <iostream>

int main()
{
    TinyScript::Lexer lexer("test.txt");
    while (!lexer.is_eof())
        std::cout << lexer.next().data << std::endl;
    return 0;
}
